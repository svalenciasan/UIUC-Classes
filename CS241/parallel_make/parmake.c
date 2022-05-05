/**
 * parallel_make
 * CS 241 - Spring 2022
 */

#include "format.h"
#include "graph.h"
#include "parmake.h"
#include "parser.h"
#include "dictionary.h"
#include "set.h"
#include "queue.h"
#include "vector.h"
#include "compare.h"
#include "callbacks.h"

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>

pthread_cond_t cond;
pthread_mutex_t mutex;

graph* mk_graph = NULL;
queue* ready_target_queue = NULL;
set* acyclic_set = NULL;
// set* goal_targets = NULL;

enum status{WAITING, READY_TO_EXECUTE, COMPLETED, FAILED};// WAITING is default

void setState(void* node, enum status state) {
    rule_t* rule = (rule_t*)graph_get_vertex_value(mk_graph, node);
    rule->state = state;
}

bool isCyclic(set* visited, void* node, set* history) { //TODO: make faster by checking if state is FAILED before recursing
    if (set_contains(visited, node)) {
        // Oh! the cycle is found
        setState(node, FAILED);

        return true;
    } else {
        // Keep history of all nodes involved
        set_add(history, node);
        // 1. Mark this node as visited
        set_add(visited, node);
        // 2. Traverse through all nodes in the reachable_nodes
        // 3. Call isCyclic() for each node
        // 4. Evaluate the return value of isCyclic()
        vector* neighbors = graph_neighbors(mk_graph, node);//Free
        for (size_t i = 0; i < vector_size(neighbors); ++i) {
            if (isCyclic(visited, vector_get(neighbors, i), history)) {
                // Oh! the cycle is found
                setState(node, FAILED);

                vector_destroy(neighbors);

                return true;
            }
        }
        vector_destroy(neighbors);
    }
    // Nope, this graph is acyclic
    set_remove(visited, node);

    return false;
}

bool areChildrenComplete(void* target) {
    vector* children = graph_neighbors(mk_graph, target);//Free
    for (size_t i = 0; i < vector_size(children); ++i) {
        void* child = vector_get(children, i);
        rule_t* child_rule = (rule_t*)graph_get_vertex_value(mk_graph, child);

        if(child_rule->state != COMPLETED) {
            vector_destroy(children);

            return false;
        }
    }
    vector_destroy(children);

    return true;
}

/**
 * @brief Alerts a targer's parents that status of child has changed
 * 
 * @param target 
 */
void alertParents(void* target) {
    rule_t* rule = (rule_t*)graph_get_vertex_value(mk_graph, target);
    if (rule->state == COMPLETED) {
        // Check to see if parents are ready to be added to queue
        vector* parents = graph_antineighbors(mk_graph, target);//Free
        for (size_t i = 0; i < vector_size(parents); ++i) {
            void* parent = vector_get(parents, i);
            // printf("%s\n", (char*)parent);

            if(areChildrenComplete(parent)) {
                // puts("children complete");
                queue_push(ready_target_queue, parent);
            }
        }
        vector_destroy(parents);
    } else if (rule->state == FAILED) {
        // Alert parents of failure all the way up the ladder
        vector* parents = graph_antineighbors(mk_graph, target);//Free
        for (size_t i = 0; i < vector_size(parents); ++i) {
            void* parent = vector_get(parents, i);

            rule_t* parent_rule = (rule_t*)graph_get_vertex_value(mk_graph, parent);
            parent_rule->state = FAILED;

            // Remove from set
            if(set_contains(acyclic_set, parent)) {
                set_remove(acyclic_set, parent);
                // To prevent last thread from blocking on queue
                if(set_empty(acyclic_set)) {
                    queue_push(ready_target_queue, NULL);
                }
            }

            alertParents(parent);
        }
        vector_destroy(parents);
    }
}

/**
 * @brief Updates the status of target beased on if needed to run
 * 
 * @param target 
 */
bool needToRun(void* target) {
    // rule_t* rule = (rule_t*)graph_get_vertex_value(mk_graph, target);

    // rule is not the name of a file on disk
    if (access(target, F_OK) != 0) {
        return true;
    }

    // Check if dependencies require it to be ran again
    vector* neighbors = graph_neighbors(mk_graph, target);//Free
    for (size_t i = 0; i < vector_size(neighbors); ++i) {
        void* dependency = vector_get(neighbors, i);

        // rule depends on another rule that is not the name of a file on disk
        if (access(dependency, F_OK) != 0) {
            return true;
        } else {
            struct stat stat_target, stat_depenency;
            // Failed
            if (stat(target, &stat_target) == -1 || stat(dependency, &stat_depenency) == -1) {
                exit(1);
                return false;//todo: error
            }
            // rule depends on another file with a NEWER change time
            if (difftime(stat_target.st_mtime, stat_depenency.st_mtime) < 0) {
                return true;
            }
        }   
    }
    vector_destroy(neighbors);

    return false;
}

bool runCommands(void* target) {
    rule_t* rule = (rule_t*)graph_get_vertex_value(mk_graph, target);

    vector* commands = rule->commands;
    for (size_t i = 0; i < vector_size(commands); ++i) {
        // Command fails
        if (system(vector_get(commands, i))) {
            rule->state = FAILED;

            return false;
        }
    }

    rule->state = COMPLETED;

    return true;
}

void add_leaves(set* visited, void* target) {
    vector* neighbors = graph_neighbors(mk_graph, target);//Free
    for (size_t i = 0; i < vector_size(neighbors); ++i) {
        add_leaves(visited, vector_get(neighbors, i));
    }

    // Is a leaf
    if (vector_empty(neighbors)) {
        // If already added no need to be added on queue again
        if (set_contains(visited, target)) {
            vector_destroy(neighbors);
            return;
        }

        rule_t* rule = (rule_t*)graph_get_vertex_value(mk_graph, target);
        rule->state = READY_TO_EXECUTE;

        set_add(visited, target);
        queue_push(ready_target_queue, target);
        // puts("HELLO");
    }
    vector_destroy(neighbors);
}

void initiate_queue(vector* targets_vect) {
    // queue_push(ready_target_queue, NULL);

    set* visited = shallow_set_create();// Free
    for (size_t i = 0; i < vector_size(targets_vect); ++i) {
        // puts("for");
        add_leaves(visited, vector_get(targets_vect, i));
    }
    set_destroy(visited);
}

void* run_thread(void* input) {
    // Identifier
    // int threadId = (long)input;
    // printf("Enter: %d\n", threadId);
    //Waiting on queue
    while(!set_empty(acyclic_set)) {
        // pthread_mutex_lock(&mutex);
        // if(queue_pull(ready_target_queue) == NULL) {
        //     queue_push(ready_target_queue, NULL);
        // }
        // pthread_mutex_unlock(&mutex);
        void* target = NULL;
        pthread_mutex_lock(&mutex);
        if (!set_empty(acyclic_set)) {
            // printf("Before queue: %d\n", threadId);
            target = queue_pull(ready_target_queue);
            // printf("After queue: %d\n", threadId);
            if (target == NULL) {
                return NULL;
            }
            set_remove(acyclic_set, target);
            // if (set_contains(goal_targets, target)) {
            //     // puts("target goal removed");
            //     set_remove(goal_targets, target);
            // }
        } else {
            // printf("Leave unlock: %d\n", threadId);
            pthread_mutex_unlock(&mutex);

            return NULL;
        }
        pthread_mutex_unlock(&mutex);
        // printf("%s\n", (char*)target);
        rule_t* rule = (rule_t*)graph_get_vertex_value(mk_graph, target);

        if (needToRun(target)) {
            // puts("running");
            rule->state = READY_TO_EXECUTE;//todo: unnecessary?
            // run target commands, updates state
            runCommands(target);
            // Alert parents
            alertParents(target);
        } else {
            // Rule doesn't need to be executed
            rule->state = COMPLETED;
            // Alert parents
            alertParents(target);
        }
    }
    // queue_push(target_queue, NULL);//Prevents blocking
    // printf("Leave: %d\n", threadId);
    return NULL;
}

int parmake(char *makefile, size_t num_threads, char **targets) {
    // Setup
    pthread_cond_init(&cond, NULL);// Free
    pthread_mutex_init(&mutex, NULL);// Free
    ready_target_queue = queue_create(-1);// Free
    // goal_targets = shallow_set_create();// Free
    acyclic_set = shallow_set_create();// Free
    mk_graph = parser_parse_makefile(makefile, targets);// Free
    vector* targets_vect = graph_neighbors(mk_graph, "");// Free

    // Looking for cycles
    set* visited = shallow_set_create();// Free
    set* history = shallow_set_create();// Free
    for (size_t i = vector_size(targets_vect); i > 0; --i) {
        void* target = vector_get(targets_vect, i - 1);
        if (isCyclic(visited, target, history)) {
            vector_erase(targets_vect, i - 1);
            print_cycle_failure(target);
        } else {
            // set_add(goal_targets, target);// Set for the final goal targets
            // Adds all acyclic targets to global set
            // puts("hello");
            vector* acyclic_targets = set_elements(history);// Free
            for (size_t j = 0; j < vector_size(acyclic_targets); ++j) {
                // puts("add");
                set_add(acyclic_set, vector_get(acyclic_targets, j));
            }
            vector_destroy(acyclic_targets);
        }
        set_clear(visited);
        set_clear(history);
    }
    set_destroy(visited);
    set_destroy(history);

    // Add leaves in queue
    // printf("%lx\n", num_threads);
    // printf("%lx\n", set_cardinality(goal_targets));
    // puts("initiate");
    initiate_queue(targets_vect);
    // puts("initiate");
    // printf("%s\n", (char*)queue_pull(ready_target_queue));
    // printf("%s\n", (char*)queue_pull(ready_target_queue));
    // Start up threads
    pthread_t threads[num_threads];
    for (size_t i = 0; i < num_threads; ++i) {
        pthread_create(threads + i, NULL, run_thread, (void*)(i + 1));
    }

    //Join Threads
    for(size_t i = 0; i < num_threads; ++i){
		pthread_join(threads[i], NULL);
	}

    // Cleanup
    queue_destroy(ready_target_queue);
    // set_destroy(goal_targets);
    set_destroy(acyclic_set);
    vector_destroy(targets_vect);
    graph_destroy(mk_graph);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}

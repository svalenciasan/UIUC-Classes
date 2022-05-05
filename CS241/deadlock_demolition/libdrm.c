/**
 * deadlock_demolition
 * CS 241 - Spring 2022
 */
#include "graph.h"
#include "libdrm.h"
#include "set.h"
#include <pthread.h>

struct drm_t {
    pthread_mutex_t m;
};

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;// Global mutex lock to make thread safe operations
graph* g = NULL;

bool isCyclic(set* visited, void* node) {
    if (set_contains(visited, node)) {
        // Oh! the cycle is found
        return true;
    } else {
        // 1. Mark this node as visited
        set_add(visited, node);
        // 2. Traverse through all nodes in the reachable_nodes
        // 3. Call isCyclic() for each node
        // 4. Evaluate the return value of isCyclic()
        vector* neighbors = graph_neighbors(g, node);
        for (size_t i = 0; i < vector_size(neighbors); ++i) {
            if (isCyclic(visited, vector_get(neighbors, i))) {
                free(neighbors);

                return true;
            }
        }
        free(neighbors);
    }
    // Nope, this graph is acyclic
    return false;
}

drm_t *drm_init() {
    /* Your code here */
    pthread_mutex_lock(&m);

    drm_t* drm = malloc(sizeof(drm_t));
    pthread_mutex_init(&drm->m, NULL);

    if (!g) {
      g = shallow_graph_create();
    }
    graph_add_vertex(g, drm);

    pthread_mutex_unlock(&m);

    return drm;
}

int drm_post(drm_t *drm, pthread_t *thread_id) {
    /* Your code here */
    pthread_mutex_lock(&m);

    if (!graph_contains_vertex(g, drm) || !graph_contains_vertex(g, thread_id)) {
        pthread_mutex_unlock(&m);
        return 0;
    }

    // Resource node to process means lock is being used by thread.
    if (graph_adjacent(g, drm, thread_id)) {
      graph_remove_edge(g, drm, thread_id);
      pthread_mutex_unlock(&drm->m);
    }

    pthread_mutex_unlock(&m);

    return 1;
}

int drm_wait(drm_t *drm, pthread_t *thread_id) {
    /* Your code here */
    pthread_mutex_lock(&m);

    graph_add_vertex(g, thread_id);

    // if (!graph_contains_vertex(g, drm)) { //todo: remove?
    //     return 0;
    // }

    // Lock already in use by thread. Deadlock
    if (graph_adjacent(g, drm, thread_id)) {
        pthread_mutex_unlock(&m);
        return 0;
    }

    graph_add_edge(g, thread_id, drm);// Requesting lock

    //Check if this creates a cycle. Creates Deadlock
    set* visited = shallow_set_create();
    if (isCyclic(visited, thread_id)) {
        graph_remove_edge(g, thread_id, drm);
        free(visited);//todo

        pthread_mutex_unlock(&m);

        return 0;
    }
    free(visited);//todo

    //No deadlock
    pthread_mutex_unlock(&m);
    pthread_mutex_lock(&drm->m);
    pthread_mutex_lock(&m);

    // todo: Removes old edge
    vector* neighbors = graph_neighbors(g, drm);//Free
    if (!vector_empty(neighbors)) {
        graph_remove_edge(g, drm, vector_get(neighbors, 0));
    }
    free(neighbors);

    graph_remove_edge(g, thread_id, drm);
    graph_add_edge(g, drm, thread_id);

    
    pthread_mutex_unlock(&m);
    return 1;
}

void drm_destroy(drm_t *drm) {
    /* Your code here */
    if (graph_contains_vertex(g, drm)) {
        graph_remove_vertex(g, drm);
    }
    //TODO global m
    pthread_mutex_destroy(&drm->m);
    free(drm);

    return;
}

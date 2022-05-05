/**
 * @file list.cpp
 * Doubly Linked List (MP 3).
 */

template <class T>
List<T>::List() { 
  // @TODO: graded in MP3.1
    head_ = NULL;
    tail_ = NULL;
    length_ = 0;
}

/**
 * Returns a ListIterator with a position at the beginning of
 * the List.
 */
template <typename T>
typename List<T>::ListIterator List<T>::begin() const {
  // @TODO: graded in MP3.1
  return List<T>::ListIterator(head_);
}

/**
 * Returns a ListIterator one past the end of the List.
 */
template <typename T>
typename List<T>::ListIterator List<T>::end() const {
  // @TODO: graded in MP3.1
  return List<T>::ListIterator(NULL);
  //return List<T>::ListIterator(tail_->next);
}


/**
 * Destroys all dynamically allocated memory associated with the current
 * List class.
 */
template <typename T>
void List<T>::_destroy() {
  // @todo Graded in MP3.1
  for(ListNode* curr_node = head_; curr_node != NULL;) {
    ListNode* next_node = curr_node->next;
    delete curr_node;
    curr_node = next_node;
  }
}

/**
 * Inserts a new node at the front of the List.
 * This function **SHOULD** create a new ListNode.
 *
 * @param ndata The data to be inserted.
 */
template <typename T>
void List<T>::insertFront(T const & ndata) {
  /// @todo Graded in MP3.1
  ListNode * newNode = new ListNode(ndata);
  newNode -> next = head_;
  newNode -> prev = NULL;

  if (head_ != NULL) {
    head_ -> prev = newNode;
  }
  head_ = newNode;
  
  if (tail_ == NULL) {
    tail_ = newNode;
  }
  
  length_++;
}

/**
 * Inserts a new node at the back of the List.
 * This function **SHOULD** create a new ListNode.
 *
 * @param ndata The data to be inserted.
 */
template <typename T>
void List<T>::insertBack(const T & ndata) {
  /// @todo Graded in MP3.1
  ListNode * newNode = new ListNode(ndata);
  newNode -> next = NULL;
  newNode -> prev = tail_;
  
  if (tail_ != NULL) {
    tail_ -> next = newNode;
  }
  tail_ = newNode;

  if (head_ == NULL) {
    head_ = newNode;
  }
  
  length_++;
}

/**
 * Helper function to split a sequence of linked memory at the node
 * splitPoint steps **after** start. In other words, it should disconnect
 * the sequence of linked memory after the given number of nodes, and
 * return a pointer to the starting node of the new sequence of linked
 * memory.
 *
 * This function **SHOULD NOT** create **ANY** new List or ListNode objects!
 *
 * This function is also called by the public split() function located in
 * List-given.hpp
 *
 * @param start The node to start from.
 * @param splitPoint The number of steps to walk before splitting.
 * @return The starting node of the sequence that was split off.
 */
template <typename T>
typename List<T>::ListNode * List<T>::split(ListNode * start, int splitPoint) {
  /// @todo Graded in MP3.1
  ListNode * curr = start;
  if (splitPoint > length_) {
    return NULL;
  }
  for (int i = 0; i < splitPoint && curr != NULL; i++) {
    curr = curr->next;
  }

  if (curr != NULL && curr->prev != NULL) {
      curr->prev->next = NULL;
      curr->prev = NULL;
  }

  return curr;
}

/**
  * Modifies List using the rules for a TripleRotate.
  *
  * This function will to a wrapped rotation to the left on every three 
  * elements in the list starting for the first three elements. If the 
  * end of the list has a set of 1 or 2 elements, no rotation all be done 
  * on the last 1 or 2 elements.
  * 
  * You may NOT allocate ANY new ListNodes!
  */
template <typename T>
void List<T>::tripleRotate() {
  // @todo Graded in MP3.1
  ListNode* curr_node = head_;
  for (size_t num_rotate = length_ / 3; num_rotate > 0; num_rotate--) {
    for (size_t i = 1; i < 3; i++) {
      if (curr_node == head_) {
        ListNode* switched = curr_node->next;
        switched->prev = curr_node->prev;
        if (curr_node->prev != NULL) {
          (curr_node->prev)->next = switched;
        }
        curr_node->prev = curr_node->next;
        curr_node->next = (curr_node->next)->next;
        switched->next = curr_node;
        if (curr_node->next != NULL) {
          (curr_node->next)->prev = curr_node;
        }
        head_ = switched;
      } else {
        ListNode* switched = curr_node->next;
        switched->prev = curr_node->prev;
        if (curr_node->prev != NULL) {
          (curr_node->prev)->next = switched;
        }
        curr_node->prev = curr_node->next;
        curr_node->next = curr_node->next->next;
        switched->next = curr_node;
        if (curr_node->next != NULL) {
          (curr_node->next)->prev = curr_node;
        }
      }
      if (curr_node->next == NULL) {
        tail_ = curr_node;
      }
    }
    curr_node = curr_node->next;
  }
}


/**
 * Reverses the current List.
 */
template <typename T>
void List<T>::reverse() {
  reverse(head_, tail_);
}

/**
 * Helper function to reverse a sequence of linked memory inside a List,
 * starting at startPoint and ending at endPoint. You are responsible for
 * updating startPoint and endPoint to point to the new starting and ending
 * points of the rearranged sequence of linked memory in question.
 *
 * @param startPoint A pointer reference to the first node in the sequence
 *  to be reversed.
 * @param endPoint A pointer reference to the last node in the sequence to
 *  be reversed.
 */
template <typename T>
void List<T>::reverse(ListNode *& startPoint, ListNode *& endPoint) {
  /// @todo Graded in MP3.2
  ListNode* start = startPoint;
  ListNode* end = endPoint;

  while (start != NULL && end != NULL && start != end &&
        (end->next) != start && (start->prev) != end) {
    //Fix Head and Tail
    if (start == head_) {
      head_ = end;
    }
    if (end == tail_) {
      tail_ = start;
    }
    //Switches ListNode pointers
    ListNode* temp_ptr = start;
    start = end;
    end = temp_ptr;

    ListNode temp_node = *start;
    start->prev = end->prev;
    //Especial case when even # of nodes
    if (end->next == start) {
      start->next = end;
    } else {
      start->next = end->next;
    }
    //Especial case when even # of nodes
    if (temp_node.prev == end) {
      end->prev = start;
    } else {
      end->prev = temp_node.prev;
    }
    end->next = temp_node.next;
    //Fix connected nodes inside
    if (start->next != NULL) {
      (start->next)->prev = start;
    }
    if (end->prev != NULL) {
      (end->prev)->next = end;
    }
    //Fix connected nodes outside
    if (start->prev != NULL) {
      (start->prev)->next = start;
    }
    if (end->next != NULL) {
      (end->next)->prev = end;
    }

    start = start->next;
    end = end->prev;
  }
}

/**
 * Reverses blocks of size n in the current List. You should use your
 * reverse( ListNode * &, ListNode * & ) helper function in this method!
 *
 * @param n The size of the blocks in the List to be reversed.
 */
template <typename T>
void List<T>::reverseNth(int n) {
  /// @todo Graded in MP3.2
  ListNode* endNode = head_;
  for (ListNode* startNode = head_; startNode != NULL; startNode = startNode->next) {
    endNode = startNode;
    for (auto i = n; i > 1; i--) {
      if(endNode->next == NULL) {
        break;
      }
      endNode = endNode->next;
    }
    reverse(startNode, endNode);
  }
}


/**
 * Merges the given sorted list into the current sorted list.
 *
 * @param otherList List to be merged into the current list.
 */
template <typename T>
void List<T>::mergeWith(List<T> & otherList) {
    // set up the current list
    head_ = merge(head_, otherList.head_);
    tail_ = head_;

    // make sure there is a node in the new list
    if (tail_ != NULL) {
        while (tail_->next != NULL)
            tail_ = tail_->next;
    }
    length_ = length_ + otherList.length_;

    // empty out the parameter list
    otherList.head_ = NULL;
    otherList.tail_ = NULL;
    otherList.length_ = 0;
}

/**
 * Helper function to merge two **sorted** and **independent** sequences of
 * linked memory. The result should be a single sequence that is itself
 * sorted.
 *
 * This function **SHOULD NOT** create **ANY** new List objects.
 *
 * @param first The starting node of the first sequence.
 * @param second The starting node of the second sequence.
 * @return The starting node of the resulting, sorted sequence.
 */
template <typename T>
typename List<T>::ListNode * List<T>::merge(ListNode * first, ListNode* second) {
  if (first == NULL) {
    return second;
  } else if (second == NULL) {
    return first;
  }

  ListNode* first_curr = first;
  ListNode* second_curr = second;

  while(first_curr != NULL) {
    if(second_curr == NULL) {
      break;
    }

    if(second_curr->data < first_curr->data) {
      //UPDATING BEGINNING OF LIST
      if(first_curr == first) {
        first = second_curr;
      }

      ListNode tmp_first = *first_curr;
      ListNode tmp_second = *second_curr;
      if (first_curr->prev != NULL) {
        (first_curr->prev)->next = second_curr;
      }
      first_curr->prev = second_curr;
      second_curr->prev = tmp_first.prev;
      second_curr->next = first_curr;

      // first_curr = second_curr;
      second_curr = tmp_second.next;
      continue;
    }
    //ADD THE REST OF 2 BREAK
    if (first_curr->next == NULL) {
      first_curr->next = second_curr;
      second_curr->prev = first_curr;
      break;
    }

    first_curr = first_curr->next;
  }
  //Check if list2 still has nodes. If so, add them to the end

  return first;
}

/**
 * Sorts a chain of linked memory given a start node and a size.
 * This is the recursive helper for the Mergesort algorithm (i.e., this is
 * the divide-and-conquer step).
 *
 * Called by the public sort function in List-given.hpp
 *
 * @param start Starting point of the chain.
 * @param chainLength Size of the chain to be sorted.
 * @return A pointer to the beginning of the now sorted chain.
 */
template <typename T>
typename List<T>::ListNode* List<T>::mergesort(ListNode * start, int chainLength) {
  /// @todo Graded in MP3.2
  if(chainLength <= 1) {
    return start;
  }
  int split_point = chainLength/2;
  ListNode* second_start = split(start, split_point);
  ListNode* second_merged = mergesort(second_start, chainLength - split_point);
  ListNode* first_merged = mergesort(start, split_point);
  return merge(first_merged, second_merged);
}
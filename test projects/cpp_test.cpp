/******************************************************************************************************************************************
 *
 * C Tools Library (CTSL)
 *
 * Copyright (C) 2022 Roland Mishaev (rmishaev@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *******************************************************************************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#ifdef _WIN32 
#include <sstream>
#include <string>
#define str_dup _strdup
#elif __linux__
#include <dlfcn.h>
#include <sys/time.h>
#define str_dup strdup
#elif __APPLE__
#define str_dup strdup
#endif

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <chrono>

#include <ctsl.h>

static const size_t N = (size_t)1e7;

#pragma region common

#define PASS(func) {\
    printf("\n--- %s started!\n", #func ); \
    bool res = func(); \
    if (res) \
        printf("--- %s passed!\n\n\n", #func ); \
    else \
        printf("--- %s failed!\n\n\n",  #func ); \
}

#pragma endregion



#pragma region map
///-------------------- map example ------------------///
static int compare_e (const void* left, const void* right) { //compare keys func example
    object_t *obj_l = (object_t *)left;
    object_t *obj_r = (object_t *)right;

    if (obj_l->type == otCharPtr && obj_r->type == otCharPtr)
    {
        return strcmp ((const char *)obj_l->data, (const char *)obj_r->data);
    }

    return memcmp(obj_l->data, obj_r->data, MIN(obj_l->size, obj_r->size));
}

void remove_key(map_t *map, const char* key){
    if (STATUS_OK == map_key_remove(map, (void *) key, strlen(key), otCharPtr)){
        printf ("Key '%s' removed\n", key);
        if (map_contains_key(map, (void *) key, strlen(key), otCharPtr)){
            printf ("ERROR: Key '%s' remove failure!\n", key);
        }else{
            printf ("VALIDATION: Key '%s' removed successfully!\n", key);
        }
    }else{
        printf ("ERROR: Key '%s' remove failure!\n", key);
    }
}

void print_map(iterator_t *iterator)
{
    if (!iterator)
        return;

    //print map content
    char key_buf[128] = {0};
    char val_buf[128] = {0};
    printf("\nMap content is:\n");


    map_reset_iterator(iterator);
    while(iterator->next(iterator)) {
        switch (iterator->key->type) {
            case otCharPtr:
                sprintf(key_buf, "\tkey/value: '%s' -> ", cust.getCharPtr(iterator->key->data));
                break;
            case otInt32:
                sprintf(key_buf, "\tkey/value: %i -> ", cust.getInt32(iterator->key->data));
                break;
            default:
                break;
        }

        switch (iterator->value->type) {
            case otCharPtr:
                sprintf(val_buf, "'%s'\n", cust.getCharPtr(iterator->value->data));
                break;
            case otInt32:
                sprintf(val_buf, "%i\n", cust.getInt32(iterator->value->data));
                break;
            default:
                break;
        }

        printf("%s%s", key_buf, val_buf);
    }
}


bool map_test() {
    const char *char_value[] = {  "A","B","C","D","E","F","G","H","I","J","K","L","M",
                                  "N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
    int int_value[] = { 1,2,3,4,5,6,7,8,9,10,
                        11,12,13,14,15,16,17,18,19,20,
                        21,22,23,24,25,26};

    int obj_value;
    map_t *map = map_create(compare_e, NULL, NULL);

    //insert and validate
    int size = sizeof(char_value)/sizeof(char_value[0]);
    for (int i = 0; i < size; ++i) {
        char *key = str_dup( char_value[i]);
        int key_length = (int)strlen ( key ) + 1;
        int value = int_value[i];
        printf ( "Inserting [%s -> %d]\n", key, value );
        map_insert(map, key, key_length, otCharPtr, &value, sizeof(int), otInt32);

        //check that item is iserted to map
        if (map_value_by_key(map, key, strlen(key), otCharPtr, &obj_value) == STATUS_OK){
            if (value == obj_value){
                printf("VALIDATION SUCCESS: The key '%s' corresponds to value [ %i ]!\n", key, value);
            }else{
                printf("VALIDATION ERROR: The key '%s' corresponds to wrong value [ %d ]!\n", key, obj_value);
            }
        }else{
            printf("VALIDATION ERROR: The key '%s' doesn't exists in map!\n", key);
        }

        free (key);
    }

    //remove some keys
    remove_key(map, "A");
    remove_key(map, "D");
    remove_key(map, "E");
    remove_key(map, "Y");


    //iterators usage
    iterator_t *iterator = map_create_iterator(map);

    //change content of each second integer element using iterator
    int value;
    bool change = false;

    printf("Mult to 10 each second element using iterator.\n");
    while(iterator->next(iterator))
    {
        if (iterator->value->type != otInt32)
            continue;

        if (change == false){
            change = true;
            continue;
        }

        change = false;
        value = cust.getInt32(iterator->value->data) * 10;
        iterator->replace_item(iterator, &value, sizeof(value), otInt32);
    }

    print_map(iterator);

    printf("Insert to string elements with ineger keys.\n");

    //add int key and string value
    printf("Mult to 10 each second element using iterator.\n");
    char* v1 = (char*)"val 1";
    int k1 = 1;
    map_insert(map, &k1, sizeof(int), otInt32, v1, strlen(v1) + 1, otCharPtr);

    k1 = 3;
    v1 = (char*)"val 2";
    map_insert(map, &k1, sizeof(int), otInt32, v1, strlen(v1) + 1, otCharPtr);

    print_map(iterator);

    map_delete_iterator(iterator);
    map_delete(map);
    return true;
}
#pragma endregion


#pragma region stl_map
bool stl_map() {
    std::map<std::string, size_t> smap;

    timer.start();
    for (size_t i = 1; i <= N; ++i) {
        smap[std::to_string(i)] = i;
    }
    double t = timer.stop(tsMilliSeconds);
    printf ("Loading %lu (map contains %lu elements) items into the STL map took %.3f milli-secs.\n", N, smap.size(), t);

    map_t *mapt = map_create(NULL, NULL, NULL);
    timer.start();
    for (size_t i = 1; i <= N; ++i) {
        const std::string& key = std::to_string(i);
        map_insert(mapt, (void *) key.c_str(), key.length() + 1, otCharPtr, &i, sizeof(size_t), otUInt64);
    }
    t = timer.stop(tsMilliSeconds);
    printf ("Loading %lu (map_st contains %lu elements) items into the  map took %.3f milli-secs.\n", N, map_size(mapt), t);

    map_delete(mapt);
    return true;
}
#pragma endregion


#pragma region vector_st

typedef struct pair{
    double val;
    double val2;
}pair_t;

static int vect_compare(const void* left, const void* right){ //compare numbers func
    pair_t *l = (pair_t*) left;
    pair_t *r = (pair_t*) right;
//    return (int)((long)l->val - (long)r->val); //sort asc
    return (int)(r->val - l->val); //sort desc
}

#ifdef STDC23
static bool cmp(const pair_t& a, const pair_t& b)
{
//    return a.val < b.val; //asc
    return b.val < a.val; //desc
}
#endif //STDC23

void print_vect(vector_st_t* vec, size_t size){
    pair_t item;

    for (size_t i = 0; i < size; ++i) {
        if (CTL_FAILED(vect_item(vec, i, (void*)&item))) {
            printf("Failed to recieve item!\n");
            return;
        }

        printf("item %lu: %f, %f\n", i, item.val, item.val2);
    }
}

bool vector_st_test()
{
//    void* item;
    size_t size = 12; // N;

    pair_t pr;
    std::vector<pair_t> V;
    V.reserve(12);

    timer.start();
    for (size_t i = 1; i <= N; ++i) {
        pr.val = (double)i;
        pr.val2 = (double)i * i;
        V.push_back(pr);
    }

    double t = timer.stop(tsMilliSeconds);
    printf ("Loading %lu (STL vector contains %lu elements) items into the STL vector took %.3f milli-secs.\n", N, V.size(), t);


#ifdef STDC23
    timer.start();
    std::stable_sort(V.begin(), V.end(), cmp);
    t = timer.stop(tsMilliSeconds);
    printf ("Sorting of %zu items in STL vector took %.3f milli-secs.\n", N, t);
#endif //STDC23

    timer.start();
    std::vector<pair_t> clone(V);
    t = timer.stop(tsMilliSeconds);
    printf ("Cloning of %zu items in STL vector took %.3f milli-secs.\n", N, t);

    timer.start();
    V.erase(V.begin() + (N >> 2), V.begin() + (N >> 2) + (N >> 1));
    t = timer.stop(tsMilliSeconds);
    printf ("Remove %lu items from STL vector took  %.3f milli-secs.\n", N >> 1, t);


    double stl_sum = 0;
    timer.start();
    for (size_t i = 0; i < V.size(); ++i) {
        stl_sum += V[i].val;
    }
    t = timer.stop(tsMilliSeconds);
    printf("Calc sum %f of the items through direct access in STL vector took %.3f milli-secs.\n", stl_sum, t);


    stl_sum = 0;
    timer.start();
    for (auto it = V.begin(); it != V.end(); ++it) {
        stl_sum += it->val;
    }
    t = timer.stop(tsMilliSeconds);
    printf ("Calc sum %f of the items through iterator in STL vector took %.3f milli-secs.\n", stl_sum, t);

////VECTOR_ST TESTS
    vector_st_t* vec = vect_create(size, sizeof(pair_t), NULL);
    validate_mem(vec, false);

    timer.start();
    for (size_t i = 1; i <= N; ++i) {
        pr.val = (double)i;
        pr.val2 = (double)i * i;
        vect_push_back(vec, (void *) &pr);
    }
    t = timer.stop(tsMilliSeconds);
    printf ("\nLoading %lu (vector contains %lu elements) items into the vector took %.3f milli-secs.\n", N, vect_size(vec), t);

    print_vect(vec, 5);

    timer.start();
    vector_st_t* clone_vec = vect_clone(vec, NULL);
    t = timer.stop(tsMilliSeconds);
    printf ("Cloning of %zu items to new vector took %.3f milli-secs.\n", N, t);
    print_vect(clone_vec, 5);
    vect_delete(clone_vec);

    timer.start();
    vect_remove_range(vec, N >> 2, N >> 1);
    t = timer.stop(tsMilliSeconds);
    printf ("Remove %lu items from vector took %.3f milli-secs.\n", N >> 1, t);

    //compare content
    pair_t* ppr;
    bool is_equ = true;
    for (size_t i = 0; i < vect_size(vec); ++i) {
        ppr = __ctl_cvt__(pair_t*, vect_item_direct(vec, i));
        pr = V[i];

        if (ppr->val != pr.val || ppr->val2 != pr.val2){
            is_equ = false;
            break;
        }
    }

    printf ("VERIFICATION: Content of the vector and STL vector is the same: %i.\n", is_equ);


    timer.start();
    vect_sort(vec, vect_compare);
    t = timer.stop(tsMilliSeconds);
    printf("Sorting of %zu items in vector took %.3f milli-secs.\n", N, t);

    print_vect(vec, 5);


    size_t i = N >> 2;
    pr.val = (double)i;
    pr.val2 = (double)i * i;

    timer.start();
    long v_ind = vect_find_item(vec, &pr, vect_compare);
    t = timer.stop(tsMilliSeconds);
    printf ("Find item at index: %li in vector took %.3f milli-secs.\n", v_ind, t);

    timer.start();
    vect_revert(vec);
    t = timer.stop(tsMilliSeconds);
    printf ("Reverting of %zu items in vector took %.3f milli-secs.\n", N, t);
    print_vect(vec, 5);


    stl_sum = 0;
    timer.start();
    size_t sz = vect_size(vec);
    for (size_t j = 0; j < sz; ++j) {
        stl_sum += __ctl_cvt__(pair_t*, vect_item_direct(vec, j))->val;
    }
    t = timer.stop(tsMilliSeconds);
    printf ("Sum %f by direct access to items in vector took %.3f milli-secs.\n", stl_sum, t);

    //iterators test
    stl_sum = 0;
    iterator_t* itr = vect_create_iterator(vec);
    timer.start();
    while (itr->next(itr)) {
        stl_sum += ((pair_t*)itr->value)->val;
    }
    t = timer.stop(tsMilliSeconds);
    printf ("Sum %f by access to items through iterator in vector took %.3f milli-secs.\n", stl_sum, t);

    vect_delete_iterator(itr);

    size_t n = vect_size(vec) >> 1;
    timer.start();
    STATUS res = vect_swap_range(vec, 0, n, n);
    t = timer.stop(tsMilliSeconds);

    if (res == STATUS_OK){
        printf ("Swap range of %lu vector items took %.3f milli-secs.\n", n, t);
        print_vect(vec, 5);

    }else{
        printf ("Swap range vector failed!\n");
    }

    //clear vector
    timer.start();
    vect_clear(vec);
    t = timer.stop(tsMilliSeconds);
    printf ("Clear vector took %.3f milli-secs.\n", t);

    if (CTL_FAILED(vect_delete(vec)))
        return false;

    return true;
}

#pragma endregion


#pragma region vector_gn

static int vec_num_compare(const void* left, const void* right){ //compare numbers func
    object_t *obj_l = (object_t *)left;
    object_t *obj_r = (object_t *)right;

    if (obj_l->type == otUInt64 &&  obj_r->type == otUInt64)
    {
        size_t *l = (size_t*) obj_l->data;
        size_t *r = (size_t*) obj_r->data;
        return *l == *r ? 0 : 1;
    }

    return 0;
}

static int vec_str_compare (const void* left, const void* right) { //compare strings func
    object_t *obj_l = (object_t *)left;
    object_t *obj_r = (object_t *)right;

    if (obj_l->type == otCharPtr &&  obj_r->type == otCharPtr)
    {
        return strcmp((const char*)obj_l->data, (const char*)obj_r->data);
    }

    return 0;
}

static int vec_sort_compare (const void* left, const void* right) { //compare objects func
    object_t *obj_l = (object_t *)left;
    object_t *obj_r = (object_t *)right;
    return memcmp(obj_r->data, obj_l->data, MIN(obj_l->size, obj_r->size));
}

void print_vecg(vector_gn_t* vec){
    object_t item;
    size_t size = vecg_size(vec);

    for (size_t i = 0; i < size; ++i) {
        if (CTL_FAILED(vecg_item(vec, i, &item))) {
            printf("Failed to recieve item!\n");
            return;
        }

        switch (item.type) {
            case otPointer:
                printf("item: %p\n", item.data);
                break;
            case otCharPtr:
                printf("item: %s\n", (char*)item.data);
                break;
            default:
                printf("item: %lu\n", *((size_t*)item.data));
                break;
        }
    }
}

void print_vecg(iterator_t *itr)
{
    while(itr->next(itr)){
        switch (itr->value->type) {
            case otPointer:
                printf("item: %p\n", cust.getPointer(itr->value->data));
                break;
            case otCharPtr:
                printf("item: %s\n", cust.getCharPtr(itr->value->data));
                break;
            default:
                printf("item: %lu\n", cust.getUint64(itr->value->data));
                break;
        }
    }
}

bool vector_gn_test()
{
    size_t size = 10;
    vector_gn_t* vec = vecg_create(12, NULL);
    validate_mem(vec, false);

    timer.start();
    for (size_t i = 1; i <= N; ++i) {
        vecg_push_back(vec, &i, sizeof(size_t), otUInt64);
    }
    double t = timer.stop(tsMilliSeconds);

    printf ("Vector load speed test: Loading %zu items into the vector took %.3f milli-secs.\n\n", N, t);

    //clear vector
    vecg_clear(vec);

    //--- int test
    for (size_t i = 0; i <= size; i++) {
        vecg_push_back(vec, &i ,sizeof(size_t), otUInt64);

        if (i == 3){
            //replace to string
            char* sval = (char*)"string value";
            vecg_replace_item(vec, i, sval, strlen(sval) + 1, otCharPtr);
        }

        if (i == 6){
            //replace to pointer fro example to vec
            vecg_replace_item(vec, i, vec, sizeof(vec), otPointer);
        }
    }

    print_vecg(vec);

    printf("\nSearch elements test:\n");
    //contains numbers test
    size_t test_val = 5;
    long contains = vecg_find_item(vec, &test_val, sizeof(size_t), otUInt64, vec_num_compare);
    printf("vector contains item = %lu: %s\n", test_val, contains > -1 ? "true" : "false");
    test_val = 20;
    contains = vecg_find_item(vec, &test_val, sizeof(size_t), otUInt64, vec_num_compare);
    printf("vector contains item = %lu: %s\n", test_val, contains > -1 ? "true" : "false");

    //contains strings test
    char* s1 = (char*)"Hello world";
    contains = vecg_find_item(vec, s1, strlen(s1) + 1, otCharPtr, vec_str_compare);
    printf("vector contains item = '%s': %s\n", s1, contains > -1 ? "true" : "false");
    s1 = (char*)"string value";
    contains = vecg_find_item(vec, s1, strlen(s1) + 1, otCharPtr, vec_str_compare);
    printf("vector contains item = '%s': %s\n", s1, contains > -1 ? "true" : "false");

    //remove single item
    vecg_remove_item(vec, 1);

    //remove items range
    printf("\nRemove element at index 1 and than 2 elements starting at index 7\n\n");
    vecg_remove_range(vec, 7, 2);
    print_vecg(vec);

    //swap elements
    printf("\nSwap element at index 1 and  element starting at index 7\n\n");
    vecg_swap(vec, 1, 7);
    print_vecg(vec);

    //swap elements range
    printf("\nSwap 3 elements range starting at index 0 and elements range starting at index 5\n\n");
    vecg_swap_range(vec, 0, 5, 3);
    print_vecg(vec);


    //revert
    printf("\nRevert array and print by Iterators\n\n");
    vecg_revert(vec);
    //iterators test
    iterator_t *itr = vecg_create_iterator(vec);
    print_vecg(itr);

    //sort vector
    printf("\nSort ascending using default comparator:\n\n");
    vecg_sort(vec, NULL);  //default compare

    vecg_reset_iterator(itr);
    print_vecg(itr);


    printf("\nSort descending using custom comparator:\n\n");
    vecg_sort(vec, vec_sort_compare); //custom compare
    vecg_reset_iterator(itr);
    print_vecg(itr);

    vecg_delete_iterator(itr);

    if (CTL_FAILED(vecg_delete(vec)))
        return false;
    return true;
}

#pragma endregion



#pragma region slist

void print_slist(iterator_t *iterator){
        if (!iterator)
        return;

        printf("\nList content is:\n");

    map_reset_iterator(iterator);
        while(iterator->next(iterator)) {
            switch (iterator->value->type) {
                case otCharPtr:
                    printf("'%s'\n", cust.getCharPtr(iterator->value->data));
                    break;
                case otInt32:
                    printf("%i\n", cust.getInt32(iterator->value->data));
                    break;
                case otUInt64:
                    printf("%lu\n", cust.getUint64(iterator->value->data));
                    break;
                default:
                    break;
            }
        }
}

bool slist_test()
{
    printf("Create new list with integers.\n");
    slist_t* list = slist_create(NULL, NULL);
    for (size_t i = 0; i < 10; ++i) {
        if (CTL_FAILED(slist_push_back(list, &i, sizeof(size_t), otUInt64)))
            return false;
    }

    iterator_t *itr = slist_create_iterator(list);
    print_slist(itr);

    printf("Insert new strings to list.\n");
    char* str = (char*)"Head";
    slist_insert(list, 0, str, strlen(str) + 1, otCharPtr);
    str = (char*)"Tail";
    slist_insert(list, 10, str, strlen(str) + 1, otCharPtr);
    str = (char*)"Middle";
    slist_insert(list, slist_size(list) / 2, str, strlen(str) + 1, otCharPtr);

    slist_reset_iterator(itr);
    print_slist(itr);


    printf("Replace value=7 with new string.\n");
    str = (char*)"Replaced";
    size_t old_val = 7;
    slist_replace_item(list, &old_val, str, strlen(str) + 1, otCharPtr);

    slist_reset_iterator(itr);
    print_slist(itr);

    slist_delete_iterator(itr);

    if (CTL_FAILED(slist_delete(list)))
        return false;

    return true;
}

#pragma endregion


#pragma region queue
void print_qus(iterator_t *iterator, size_t elm_count){
    if (!iterator)
        return;

    printf("\nQueue content is (first %lu items):\n", elm_count);

    size_t ind = 0;
    qus_reset_iterator(iterator);
    while(iterator->next(iterator)) {
        printf("%lu\n", cust.getUint64(iterator->value));

        if (++ind == elm_count)
            break;
    }
}

void print_from_ind_qus(queue_st_t* queue, size_t start_index, size_t elm_count){
    if (!queue)
        return;

    printf("\nQueue content is (first %lu items):\n", elm_count);

    for (size_t i = 0; i < elm_count; ++i) {
        printf("index:%lu   value: %lu\n", start_index + i, cust.getUint64(qus_item(queue, start_index + i)));
    }
}

#define use_stl
//#define use_stl_priority

bool queue_s_test()
{

    size_t val;
    size_t cou;

#ifdef use_stl
    //STL
#ifdef use_stl_priority
    std::priority_queue<size_t> stl_queue;
#else
    std::queue<size_t> stl_queue;
#endif
    printf("Create new STL queue and fill it with integers.\n");
    timer.start();
    for (size_t i = 0; i < N; ++i) {
        stl_queue.push(i);
    }
    double t = timer.stop(tsMilliSeconds);
    printf ("Loading %lu (STL queue contains %lu elements) items into the queue took %.3f milli-secs.\n", N, stl_queue.size(),t);
    //---- STL
#endif

    printf("\nCreate new auto-size queue_st and fill it with integers.\n");
    queue_st_t* queue = qus_create(10, sizeof(size_t), true);

    timer.start();
    for (size_t i = 0; i < N; ++i) {
        qus_push(queue, &i);
    }
    t = timer.stop(tsMilliSeconds);
    printf ("Loading %lu (queue contains %lu elements) items into the queue took %.3f milli-secs.\n", N, qus_size(queue),t);

#ifdef use_stl
    //validation
    bool equal = true;
    for (size_t i = 0; i < N; ++i) {
        val = cust.getUint64(qus_item(queue, i));
#ifdef use_stl_priority
        equal = val == stl_queue.top();
#else
        equal = val == stl_queue.front();
#endif
        stl_queue.pop();
        if (!equal)
            break;
    }
    printf ("\nContent is equal: %s.\n\n", equal ? "true" : "false");
#endif

    iterator_t* iterator = qus_create_iterator(queue);
    print_qus(iterator, 5);

    printf ("Pop first 10 elements:\n");
    for (int i = 0; i < 10; ++i) {
        val = cust.getUint64(qus_pop(queue));
        printf ("Element: %lu (queue contains %lu elements).\n", val, qus_size(queue));
    }
    print_qus(iterator, 5);

    void* v = NULL;
    cou = qus_size(queue);

    timer.start();
    do{
        v = qus_pop(queue);
    }while(v);

    t = timer.stop(tsMilliSeconds);
    printf("Pop all  %lu elements took  %.3f milli-secs  (queue contains %lu elements).\n", cou, t, qus_size(queue));

    timer.start();
    for (size_t i = 0; i < N; ++i) {
        qus_push(queue, &i);
    }
    t = timer.stop(tsMilliSeconds);
    printf ("Loading again %lu (queue contains %lu elements) items into the queue took %.3f milli-secs.\n", N, qus_size(queue),t);

    cou = 10;
    printf("Print last %lu elements", cou);
    print_from_ind_qus(queue, qus_size(queue) - cou, cou);

    qus_delete_iterator(iterator);
    qus_delete(&queue);

#ifdef use_stl \
//STL
    timer.start();
    for (size_t i = 0; i < N; ++i) {
        stl_queue.push(i);
    }
    t = timer.stop(tsMilliSeconds);
    printf ("Loading again %lu (STL queue contains %lu elements) items into the queue took %.3f milli-secs.\n", N, stl_queue.size(),t);

    cou = stl_queue.size();
    timer.start();
    do{
#ifdef use_stl_priority
        equal = val == stl_queue.top();
#else
        equal = val == stl_queue.front();
#endif
        stl_queue.pop();
    }while(!stl_queue.empty());
    t = timer.stop(tsMilliSeconds);
    printf("Pop all  %lu elements took  %.3f milli-secs  (queue contains %lu elements).\n", cou, t, stl_queue.size());
    //---- STL
#endif
    return true;
}

void print_qug(iterator_t *iterator, size_t elm_count){
    if (!iterator)
        return;

    printf("\nQueue content is (first %lu items):\n", elm_count);

    size_t ind = 0;
    qug_reset_iterator(iterator);

    while(iterator->next(iterator)) {
        switch (iterator->value->type) {
            case otCharPtr:
                printf("'%s'\n", cust.getCharPtr(iterator->value->data));
                break;
            case otUInt64:
                printf("%lu\n", cust.getUint64(iterator->value->data));
                break;
            default:
                break;
        }

        if (++ind == elm_count)
            break;
    }
}

bool queue_g_test()
{
    printf("Create new auto-size queue_gn and fill it with integers and strings.\n");
    queue_gn_t* queue = qug_create(10, true);

    std::string s;
    timer.start();
    for (size_t i = 0; i < N; ++i) {
        if (i % 2){
            s = std::to_string(i);
            qug_push(queue, (void*)s.c_str(), s.length(), otCharPtr);
        }else
            qug_push(queue, &i, sizeof(size_t), otUInt64);
    }

    double t = timer.stop(tsMilliSeconds);
    printf ("Loading %lu (queue contains %lu elements) items into the queue took %.3f milli-secs.\n", N, qug_size(queue),t);
    iterator_t* iterator = qug_create_iterator(queue);
    print_qug(iterator, 5);

    size_t cou = qug_size(queue);

    object_t* v = NULL;
    timer.start();
    do{
        v = qug_pop(queue);
    }while(v);

    t = timer.stop(tsMilliSeconds);
    printf("Pop all  %lu elements took  %.3f milli-secs  (queue contains %lu elements).\n", cou, t, qug_size(queue));

    qug_delete_iterator(iterator);
    qug_delete(&queue);
    return true;
}

#pragma endregion

bool test_string()
{
    string_a* w2a = stra_from_wstr(L"w_str"); //from unicode
    validate_mem(w2a, false);
    printf("Create instance from UNICODE string, content is: %s\n", stra_c_str(w2a));
    stra_delete(&w2a);

    string_a* str = stra_from_str("Hello ctl world!");
    printf("string content is: '%s'\n", stra_c_str(str));
    printf("remove content form index 6 and length 4\n");
    stra_remove_range(str, 6, 4);
    printf("string content is: '%s', string length is: %lu\n", stra_c_str(str), stra_length(str));
    printf("Append symbol '!'\n");
    stra_append_chr(str, '!');
    printf("string content is: '%s'\n", stra_c_str(str));
    printf("Append string 'I'm here.'\n");
    stra_append_str(str, "\nI'm here.");
    printf("string content is: '%s'\n", stra_c_str(str));

    string_a* str2 = stra_from_str("Now lets do something new!");
    printf("is content of two objects the same: '%s'\n", stra_compare_stra(str, str2) == 0 ? "true" : "false");

    printf("assign string object\n");
    stra_assign_stra(str, str2);
    printf("string content is: '%s'\n", stra_c_str(str));
    printf("is content of two objects the same: '%s'\n", stra_compare_stra(str, str2) == 0 ? "true" : "false");

    printf("string contains word 'world': '%s'\n", stra_contains_str(str, "world") ? "true" : "false");
    printf("string contains word 'new': '%s'\n", stra_contains_str(str, "new") ? "true" : "false");

    printf("first 'o' symbol is at pos: '%lu'\n", stra_find_first_chr(str, 'o'));
    printf("last 'o' symbol is at pos: '%lu'\n", stra_find_last_chr(str, 'o'));

    printf("Append string 'Search the 'new'e word.'\n");
    stra_append_str(str, " Search the 'new' word");
    printf("string content is: '%s'\n", stra_c_str(str));

    printf("first 'new' word is at pos: '%lu'\n", stra_find_first_str(str, "new"));
    printf("last 'new' word is at pos: '%lu'\n", stra_find_last_str(str, "new"));

    printf("content starts with 'now': '%s'\n", stra_starts_with_str(str, "now") ? "true" : "false");
    printf("content starts with 'N': '%s'\n", stra_starts_with_chr(str, 'N') ? "true" : "false");
    printf("content ends with 'word': '%s'\n", stra_ends_with_str(str, "word") ? "true" : "false");
    printf("content ends with 'D': '%s'\n", stra_ends_with_chr(str, 'D') ? "true" : "false");

    if (stra_erase(str, 21, 4) == STATUS_OK){
        printf("Remove first 'new' word.\nstring content is:\n\t'%s', \n\tstring length is: %lu\n", stra_c_str(str), stra_length(str));
    }

    string_a* str3 = stra_from_str("0123456789");
    printf("create new string, string content is: '%s'\n: ", stra_c_str(str3));
    stra_reverse(str3);
    printf("reverse string, string content is: '%s'\n", stra_c_str(str3));
    stra_reverse_range(str3, 4, 3);
    printf("reverse range pos=4, len=3 string, string content is: '%s'\n", stra_c_str(str3));

    string_a* str4 = stra_substr(str, 4, 18);
    printf("substring from pos=4 and len=18 is: '%s'\n", stra_c_str(str4));

    stra_insert_str(str4, " interesing", 17);
    printf("insert ' interesing' to pos=17: '%s'\n", stra_c_str(str4));
    stra_insert_chr(str4, '!', 28);
    printf("insert '!' to pos=28: '%s'\n", stra_c_str(str4));

    string_a* str5 = stra_add_chr(str3, 'A');
    printf("new object from exising obejet and char 'A' : '%s'\n", stra_c_str(str5));
    string_a* str6 = stra_add_stra(str3, str4);
    printf("new object from exising obejets : '%s'\n", stra_c_str(str6));

    stra_swap_range(str6, 0, 10, 10);
    printf("swap first 10 symbols with next 10 symbols: '%s'\n", stra_c_str(str6));

    stra_replace_chr(str6, 0, 7, '@', 1);
    printf("replace first 7 symbols with '@' symbol: '%s'\n", stra_c_str(str6));
    stra_replace_chr(str6, 32, 2, '.', 5);
    printf("replace '!!' with five '.' symbol: '%s'\n", stra_c_str(str6));

    stra_replace_strex(str6, 0, 21, "so,", 0, 2);
    printf("replace first 21 symbols with sub-string of string 'so, ' symbol: '%s'\n", stra_c_str(str6));
    stra_replace_str(str6, 2, 15, ", we can work with strings in simple way");
    printf("finally replace sub-string pos=2, len=15 with new string : '%s'\n", stra_c_str(str6));

    // num to str and back
    stra_assign_str(str, "3.14159265358979323846");
    double pi_d = stra_stod(str);
    long double pi_ld = stra_stold(str);

    printf("pi double value: %.20f\n", pi_d);
    printf("pi long double value: %.20Lf\n", pi_ld);

    //num to str
    string_a* str7 = stra_dtos(pi_ld);
    printf("string from double value: %s\n", stra_c_str(str7));
    string_a* str8 = stra_ldtos(pi_ld);
    printf("string from long double value: %s\n", stra_c_str(str8));

    //splitter test
    printf("\nLet's split the string by ' ' delimeter:\n");
    vector_st_t* splitter = stra_create_splitter_chr(str6, ' ');

    //we can use iterator to enumerate the splitted string context
    iterator_t* itr = vect_create_iterator(splitter);

    //get content of the splitter
    while(itr->next(itr)){
        printf("\t%s\n", *(char**)itr->value);
    }

    stra_delete_splitter(splitter);
    vect_delete_iterator(itr);

    printf("\nLet's split the same string by ', ' delimeter:\n");
    splitter = stra_create_splitter_str(str6, ", ");

    //another way to get content of the splitter
    for (size_t i = 0; i < vect_size(splitter); ++i) {
        const char** item = (const char**)vect_item_direct(splitter, i);
        printf("\t%s\n", *item);
    }

    stra_delete_splitter(splitter);

    stra_delete(&str);
    stra_delete(&str2);
    stra_delete(&str3);
    stra_delete(&str4);
    stra_delete(&str5);
    stra_delete(&str6);
    stra_delete(&str7);
    stra_delete(&str8);

    return true;
}

bool test_string_w()
{
    string_w* a2w = strw_from_astr("a_str");
    printf("Create instance from ANSI string, content is: %ls\n", strw_c_str(a2w));
    strw_assign_str(a2w, L"-1234567890");
    long v = strw_stol(a2w);
    printf("Assign -1234567890 to string_w and convert to long integer, content is: %li\n", v);
    strw_delete(&a2w);

    string_w* str = strw_from_str(L"Hello ctl world!");
    printf("string content is: '%ls'\n", strw_c_str(str));
    printf("remove content form index 6 and length 4\n");
    strw_remove_range(str, 6, 4);
    printf("string content is: '%ls', string length is: %lu\n", strw_c_str(str), strw_length(str));
    printf("Append symbol L'!'\n");
    strw_append_chr(str, L'!');
    printf("string content is: '%ls'\n", strw_c_str(str));
    printf("Append string 'I'm here.'\n");
    strw_append_str(str, L"\nI'm here.");
    printf("string content is: '%ls'\n", strw_c_str(str));

    string_w* str2 = strw_from_str(L"Now lets do something new!");
    printf("create new string: '%ls'\n", strw_c_str(str2));
    printf("is content of two objects the same: '%s'\n", strw_compare_strw(str, str2) == 0 ? "true" : "false");

    printf("assign second string to first string object\n");
    strw_assign_strw(str, str2);
    printf("string content is: '%ls'\n", strw_c_str(str));
    printf("is content of two objects the same: '%s'\n", strw_compare_strw(str, str2) == 0 ? "true" : "false");

    printf("string contains word 'world': '%s'\n", strw_contains_str(str, L"world") ? "true" : "false");
    printf("string contains word 'new': '%s'\n", strw_contains_str(str, L"new") ? "true" : "false");

    printf("first 'o' symbol is at pos: '%lu'\n", strw_find_first_chr(str, L'o'));
    printf("last 'o' symbol is at pos: '%lu'\n", strw_find_last_chr(str, L'o'));

    printf("Append string 'Search the 'new' word.'\n");
    strw_append_str(str, L" Search the 'new' word");
    printf("string content is: '%ls'\n", strw_c_str(str));

    printf("first 'new' word is at pos: '%lu'\n", strw_find_first_str(str, L"new"));
    printf("last 'new' word is at pos: '%lu'\n", strw_find_last_str(str, L"new"));

    printf("content starts with 'now': '%s'\n", strw_starts_with_wstr(str, L"now") ? "true" : "false");
    printf("content starts with 'N': '%s'\n", strw_starts_with_wchr(str, L'N') ? "true" : "false");
    printf("content ends with 'word': '%s'\n", strw_ends_with_wstr(str, L"word") ? "true" : "false");
    printf("content ends with 'D': '%s'\n", strw_ends_with_wchr(str, L'D') ? "true" : "false");

    if (strw_erase(str, 21, 4) == STATUS_OK){
        printf("Remove first 'new' word.\nstring content is:\n\t'%ls', \n\tstring length is: %lu\n", strw_c_str(str), strw_length(str));
    }

    string_w* str3 = strw_from_str(L"0123456789");
    printf("create new string, string content is: '%ls'\n: ", strw_c_str(str3));
    strw_reverse(str3);
    printf("reverse string, string content is: '%ls'\n", strw_c_str(str3));
    strw_reverse_range(str3, 4, 3);
    printf("reverse range pos=4, len=3 string, string content is: '%ls'\n", strw_c_str(str3));

    string_w* str4 = strw_substr(str, 4, 18);
    printf("substring from pos=4 and len=18 is: '%ls'\n", strw_c_str(str4));

    strw_insert_str(str4, L" interesing", 17);
    printf("insert ' interesing' to pos=17: '%ls'\n", strw_c_str(str4));
    strw_insert_chr(str4, L'!', 28);
    printf("insert '!' to pos=28: '%ls'\n", strw_c_str(str4));

    string_w* str5 = strw_add_chr(str3, L'A');
    printf("new object from exising obejet and char 'A' : '%ls'\n", strw_c_str(str5));
    string_w* str6 = strw_add_strw(str3, str4);
    printf("new object from exising obejets : '%ls'\n", strw_c_str(str6));

    strw_swap_range(str6, 0, 10, 10);
    printf("swap first 10 symbols with next 10 symbols: '%ls'\n", strw_c_str(str6));

    strw_replace_chr(str6, 0, 7, L'@', 1);
    printf("replace first 7 symbols with '@' symbol: '%ls'\n", strw_c_str(str6));
    strw_replace_chr(str6, 32, 2, '.', 5);
    printf("replace '!!' with five '.' symbol: '%ls'\n", strw_c_str(str6));

    strw_replace_strex(str6, 0, 21, L"so,", 0, 2);
    printf("replace first 21 symbols with sub-string of string 'so, ' symbol: '%ls'\n", strw_c_str(str6));
    strw_replace_str(str6, 2, 15, L", we can work with strings in simple way");
    printf("finally replace sub-string pos=2, len=15 with new string : '%ls'\n", strw_c_str(str6));

    // num to str and back
    strw_assign_str(str, L"3.14159265358979323846");
    double pi_d = strw_stod(str);
    long double pi_ld = strw_stold(str);

    printf("pi double value: %.20f\n", pi_d);
    printf("pi long double value: %.20Lf\n", pi_ld);

    //num to str
    string_w *str7 = strw_dtos(pi_ld);
    printf("string from double value: %ls\n", strw_c_str(str7));
    string_w* str8 = strw_ldtos(pi_ld);
    printf("string from long double value: %ls\n", strw_c_str(str8));


    //splitter test
    vector_st_t* splitter = NULL;
    printf("\nLet's split the string by ' ' delimeter:\n");
    splitter = strw_create_splitter_chr(str6, L' ');

    //we can use iterator to enumerate the splitted string context
    iterator_t* itr = vect_create_iterator(splitter);

    // get content of the splitter
    while(itr->next(itr)){
        printf("\t%ls\n", *(wchar_t **)itr->value);
    }

    strw_delete_splitter(splitter);
    vect_delete_iterator(itr);

    printf("\nLet's split the same string by ', ' delimeter:\n");
    splitter = strw_create_splitter_str(str6, L", ");

    //another way to get content of the splitter
    for (size_t i = 0; i < vect_size(splitter); ++i) {
        const wchar_t** item = (const wchar_t**)vect_item_direct(splitter, i);
        printf("\t%ls\n", *item);
    }

    stra_delete_splitter(splitter);

    strw_delete(&str);
    strw_delete(&str2);
    strw_delete(&str3);
    strw_delete(&str4);
    strw_delete(&str5);
    strw_delete(&str6);
    strw_delete(&str7);
    strw_delete(&str8);

    return true;
}

#ifdef _WIN32

typedef __declspec(align(64)) double* aligned_double;

void maxArray(aligned_double __restrict x, aligned_double __restrict y) {
    for (int i = 0; i < 65536; i++) {
        x[i] = ((y[i] > x[i]) ? y[i] : x[i]);
    }
}

void test_arr() {
    aligned_double x = new double[65536];
    aligned_double y = new double[65536];

    for (size_t i = 0; i < 65536; i++)
    {
        x[i] = i * 0.1;
        y[i] = i * 0.5;
    }

    maxArray(x, y);
}
#endif

int main(){
    PASS(vector_st_test);
    PASS(vector_gn_test);
    PASS(map_test);
    PASS(stl_map);
    PASS(slist_test);
    PASS(queue_s_test);
    PASS(queue_g_test);
    PASS(test_string);
    PASS(test_string_w);

    return 0;
}


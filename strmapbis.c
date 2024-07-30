
#include "strmapbis.h"
#include <stdio.h> // for printf, ONLY IN strmap_dump
#include <stdlib.h>

int hash_array(char *key)
{
  if (key == NULL)
  {
    return 0;
  }
  unsigned long index = 12345;
  for (key; (*key) != NULL; key++)
  {
    index += *key;
    index *= 7;
  }
  int int_index = index;
  return abs(int_index);
}

/* return the current load factor of the map  */
double strmap_getloadfactor(strmap_t *m)
{
  double loadfac = ((double)strmap_getsize(m) / (double)strmap_getnbuckets(m));
  return loadfac;
}

strmap_t *strmap_create(int numbuckets)
{

  if (numbuckets <= MIN_BUCKETS)
  {
    numbuckets = MIN_BUCKETS;
  }
  else if (numbuckets >= MAX_BUCKETS)
  {
    numbuckets = MAX_BUCKETS;
  }
  strmap_t *map_ptr = (strmap_t *)malloc(sizeof(strmap_t));

  map_ptr->strmap_buckets = (smel_t **)calloc(numbuckets, sizeof(smel_t *));

  /*
    for (int i = 0; i < numbuckets; i++)
    {
      map_ptr->strmap_buckets[i] = (smel_t *)calloc(1, sizeof(smel_t));
    }
  */
  // map_ptr->strmap_nbuckets = (int) malloc(sizeof(int));
  // map_ptr->strmap_size = (int) malloc(sizeof(int));
  map_ptr->strmap_size = 0;
  map_ptr->strmap_nbuckets = numbuckets;
  return map_ptr;
}

/* Insert an element with the given key and value.
 *  Return the previous value associated with that key, or null if none.
 */
void *strmap_put(strmap_t *m, char *key, void *value)
{
  int index = hash_array(key);
  int numbuckets = strmap_getnbuckets(m);
  index = index % numbuckets;
  // if bucket is empty
  if (key == NULL)
  {
    // declare variables for smel_t
    char *new_key = NULL;
    void *new_value = malloc(sizeof(void *));
    new_value = value;

    // declare ptr to point to hold null index
    smel_t *null_ptr = (smel_t *)malloc(sizeof(smel_t));
    smel_t new_smel_t = {new_key, new_value, (m->strmap_buckets)[index]};
    *null_ptr = new_smel_t;
    // set array to point to null index
    (m->strmap_buckets)[index] = null_ptr;
    m->strmap_size += 1;
    return NULL;
  }

//if bucket is empty
  if (!(m->strmap_buckets)[index])
  {
    char *new_key = strdup(key);
    void *new_value = malloc(sizeof(void *));
    new_value = value;
    smel_t *this_ptr = (smel_t *)calloc(1, sizeof(smel_t));

    smel_t new_smel_t = {new_key, new_value, NULL};
    *this_ptr = new_smel_t;
    (m->strmap_buckets)[index] = this_ptr;
    m->strmap_size += 1;
    return NULL;
  }

  // search until find something greater than
  smel_t *curr_index = (m->strmap_buckets)[index];
  smel_t *prev_index = NULL;
  // check if ptr is null

  while (curr_index && strcmp(key, (*curr_index).sme_key) > 0)
  {
    prev_index = curr_index;
    curr_index = (*curr_index).sme_next;
  }

  // equal/ same word
  if (curr_index && strcmp(key, (*curr_index).sme_key) == 0)
  {
    void *prev_value = curr_index->sme_value;
    (*curr_index).sme_value = value;
    return prev_value;
  }

  // key is greater than index

  char *new_key = strdup(key);

  void *new_value = malloc(sizeof(void *));
  new_value = value;
  // if prev_index exists
  // update prev index to point to this
  if (prev_index)
  {

    // give new smel_t pointer to next point
    smel_t new_smel_t = {new_key, new_value, prev_index->sme_next};

    // create pointer to current index
    smel_t *this_ptr = (smel_t *)malloc(sizeof(smel_t));
    *this_ptr = new_smel_t;
    // update prev pointer
    (*prev_index).sme_next = this_ptr;
  }
  // if prev_index does not exist
  else
  {
    // set pointer to first element
    smel_t new_smel_t = {new_key, new_value, curr_index};

    // create pointer to current index
    smel_t *this_ptr = (smel_t *)malloc(sizeof(smel_t));
    *this_ptr = new_smel_t;
    (m->strmap_buckets)[index] = this_ptr;
  }
  m->strmap_size += 1;

  return NULL;
}

/* return the value associated with the given key, or null if none */
void *strmap_get(strmap_t *m, char *key)
{
  int index = hash_array(key);
  index %= m->strmap_nbuckets;
  smel_t *curr_index = (m->strmap_buckets)[index];
  // check if exists
  if (!curr_index)
  {
    return NULL;
  }
  // search for the correct value
  // search until find something greater than
  while (curr_index && strcmp(key, curr_index->sme_key) != 0)
  {
    curr_index = curr_index->sme_next;
    // check if exists
    if (!curr_index)
    {
      return NULL;
    }
  }
  return curr_index->sme_value;
}

/* remove the element with the given key and return its value.
   Return null if the hashtab contains no element with the given key */
void *strmap_remove(strmap_t *m, char *key)
{
  int index = hash_array(key);
  int numbuckets = strmap_getnbuckets(m);
  index = index % numbuckets;
  void *return_value;
  // if bucket is empty
  if (!(m->strmap_buckets)[index])
  {
    return NULL;
  }

  // search until find something greater than
  smel_t *curr_index = (m->strmap_buckets)[index];
  smel_t *prev_index = NULL;

  while ((*curr_index).sme_value != NULL && strcmp(key, (*curr_index).sme_key) != 0)
  {
    prev_index = curr_index;
    curr_index = curr_index->sme_next;
  }
  // if prev index exists
  if (prev_index != NULL)
  {
    return_value = curr_index->sme_value;
    prev_index->sme_next = curr_index->sme_next;
  }
  // if it dne
  else
  {
    return_value = curr_index->sme_value;
    m->strmap_buckets[index] = curr_index->sme_next;
    //*curr_index = *(curr_index->sme_next);
  }

  m->strmap_size--;
  return return_value;
}
/* return the # of elements in the hashtab */
int strmap_getsize(strmap_t *m)
{
  return m->strmap_size;
}

/* return the # of buckets in the hashtab */
int strmap_getnbuckets(strmap_t *m)
{
  return m->strmap_nbuckets;
}
static int strmap_getmorebuckets(strmap_t *m, int guess_buckets, double target)
{
  // create load factor with new guess_buckets
  double new_loadf = (double)strmap_getsize(m) / guess_buckets;

  // increment by values of 0.1 * guess_buckets until desired load fac is reached
  while (!((((1 - .125) * target) <= new_loadf) && (((1 + .125) * target) >= new_loadf)))
  {
    if ((1 - 0.125) * target >= new_loadf)
    {
      guess_buckets *= 0.9;
    }
    if ((1 + 0.125) * target <= new_loadf)
    {
      guess_buckets *= 1.1;
    }
    new_loadf = (double)strmap_getsize(m) / guess_buckets;
  }
  // apply once more for rounding
  new_loadf = (double)strmap_getsize(m) / guess_buckets;
  if (target <= new_loadf)
  {
    guess_buckets *= 1.1;
  }
  else
  {
    guess_buckets *= 0.9;
  }
  return guess_buckets;
}

void strmap_resize(strmap_t *m, double target)
{
  // check if within restraints

  // if not in restraints
  // get new array
  if (!(((1 - LFSLOP) * target <= strmap_getloadfactor(m)) && ((1 + LFSLOP) * target >= strmap_getloadfactor(m))))
  {
    int numbuckets = strmap_getmorebuckets(m, m->strmap_nbuckets, target);
    // check edge cases
    if (numbuckets <= MIN_BUCKETS)
    {
      numbuckets = MIN_BUCKETS;
    }
    else if (numbuckets >= MAX_BUCKETS)
    {
      numbuckets = MAX_BUCKETS;
    }
    // create new ptr for map
    smel_t **new_map_ptr = (smel_t **)calloc(numbuckets, sizeof(smel_t *));
    
    for (int i = 0; i < numbuckets; i++)
    {
      if ((new_map_ptr)[i])
      {
        printf("bucket %d:\n\t%s->%p \n", i, (new_map_ptr)[i]->sme_key, (new_map_ptr)[i]->sme_value);
      }
    }

    // old number of buckets
    int old_nbuckets = m->strmap_nbuckets;

    // set nbuckets to new #
    m->strmap_nbuckets = numbuckets;

    // get pointer from old pointer
    smel_t **old_map_ptr = m->strmap_buckets;

    // free(m->strmap_buckets);
    //  set arr_ptr to new ptr
    m->strmap_buckets = new_map_ptr;

    // set size to zero
    m->strmap_size = 0;

    // trace through array and re-allocate them to new array
    for (int i = 0; i < old_nbuckets; i++)
    {

      smel_t *curr_index = old_map_ptr[i];
        while (curr_index!= NULL)
        {
          strmap_put(m, curr_index->sme_key, curr_index->sme_value);
          curr_index = curr_index->sme_next;
        }
        // put last value in
        // strmap_put(m, curr_index->sme_key, curr_index->sme_value);
      
    }
  }
  return;
}

/* print out the contents of each bucket */
void strmap_dump(strmap_t *m)
{
  printf("total elements = %i\n", strmap_getsize(m));
  for (int i = 0; i < m->strmap_nbuckets; i++)
  {
    smel_t *curr_index = m->strmap_buckets[i];
    while (curr_index)
    {
      printf("bucket %d:\n\t%s->%p \n", i, curr_index->sme_key, curr_index->sme_value);
      curr_index = curr_index->sme_next;
    }
  }
}

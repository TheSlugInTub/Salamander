#include <salamander/umap.h>
#include <stdbool.h>

smUmap* smUmap_Create(size_t keySize, size_t valueSize,
                      size_t initialCapacity)
{
    smUmap* umap = (smUmap*)malloc(sizeof(smUmap));
    if (umap == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for umap.\n");
        return NULL;
    }

    umap->data = malloc(initialCapacity * keySize * valueSize);
    if (umap->data == NULL)
    {
        free(umap);
        fprintf(stderr, "Failed to allocate memory for umap data.\n");
        return NULL;
    }

    umap->keySize = keySize;
    umap->valueSize = valueSize;
    umap->size = 0;
    umap->capacity = initialCapacity;
    return umap;
}

int smUmap_PushBack(smUmap* umap, const void* key, const void* value)
{
    // Resize if needed
    if (umap->size >= umap->capacity)
    {
        size_t new_capacity = umap->capacity * 2;
        void** new_data = realloc(
            umap->data, new_capacity * sizeof(void*) *
                            2); // Need space for key-value pairs
        if (new_data == NULL)
        {
            fprintf(stderr,
                    "Failed to reallocate memory for umap data.\n");
            return -1;
        }

        umap->data = new_data;
        umap->capacity = new_capacity;
    }

    // Allocate memory for the key and value
    void* key_copy = malloc(umap->keySize);
    void* value_copy = malloc(umap->valueSize);

    if (key_copy == NULL || value_copy == NULL)
    {
        free(key_copy); // Free if allocated (will be safe even if
                        // NULL)
        free(value_copy);
        fprintf(stderr,
                "Failed to allocate memory for key-value pair.\n");
        return -1;
    }

    // Copy key and value
    memcpy(key_copy, key, umap->keySize);
    memcpy(value_copy, value, umap->valueSize);

    // Store key and value pointers in the data array
    umap->data[umap->size * 2] = key_copy;
    umap->data[umap->size * 2 + 1] = value_copy;

    umap->size++;
    return 0;
}

void* smUmap_Get(smUmap* umap, const void* key)
{
    // Iterate through all key-value pairs
    for (size_t i = 0; i < umap->size; i++)
    {
        // Get pointer to the current key
        void* current_key = umap->data[i * 2];

        // Compare the current key with the search key
        if (memcmp(current_key, key, umap->keySize) == 0)
        {
            // Key found, return pointer to the corresponding value
            return umap->data[i * 2 + 1];
        }
    }

    // Key not found
    fprintf(stderr, "Key not found in umap.\n");
    return NULL;
}

int smUmap_Remove(smUmap* umap, const void* key)
{
    size_t index;
    bool   found = false;

    for (index = 0; index < umap->size; index++)
    {
        void* current_key = umap->data[index * 2];

        if (memcmp(current_key, key, umap->keySize) == 0)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        fprintf(stderr, "Key not found in umap.\n");
        return -1;
    }

    // Free the memory for key and value
    free(umap->data[index * 2]);     // Free key
    free(umap->data[index * 2 + 1]); // Free value

    // Shift the remaining elements
    if (index < umap->size - 1)
    {
        // Move all subsequent key-value pairs one position up
        memmove(&umap->data[index * 2], &umap->data[(index + 1) * 2],
                (umap->size - index - 1) * 2 * sizeof(void*));
    }

    umap->size--;
    return 0;
}

void smUmap_Free(smUmap* umap)
{
    free(umap->data);
    free(umap);
}

void smUmap_Clear(smUmap* umap)
{
    umap->size = 0;
}

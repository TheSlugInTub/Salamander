#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    void** data; // Array of void pointers to store any type
    size_t keySize;
    size_t valueSize;
    size_t size;     // Current number of elements in the umap
    size_t capacity; // Amount of data allocated for the umap
} smUmap;

// Initialize the umap
smUmap* smUmap_Create(size_t keySize, size_t valueSize, size_t initialCapacity);

// Push an element into a umap
int smUmap_PushBack(smUmap* umap, const void* key, const void* value);

// Get the data at an index of the umap
void* smUmap_Get(smUmap* umap, const void* key);

// Remove an element from the umap by index
int smUmap_Remove(smUmap* umap, const void* key);

// Free the memory used by the umap
void smUmap_Free(smUmap* umap);

// Set the size to zero, the memory remains allocated for future use
void smUmap_Clear(smUmap* umap);

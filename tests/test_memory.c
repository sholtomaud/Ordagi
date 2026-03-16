#include "test_runner.h"
#include "ordinal_tensor.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int save_ontology_cbor(const char *filepath, OrdinalConcept **kb, int kb_size);
int load_ontology_cbor(const char *filepath, OrdinalConcept **kb, int *kb_size);

static void test_memory_roundtrip(void) {
    OrdinalConcept *kb_save[MAX_CONCEPTS] = {NULL};
    OrdinalConcept *kb_load[MAX_CONCEPTS] = {NULL};
    const char *test_file = "data/test_ontology.cbor";

    // Create a seed concept
    OrdinalConcept *c1 = calloc(1, sizeof(OrdinalConcept));
    c1->id = 1;
    strncpy(c1->name, "solar energy", MAX_NAME_LEN-1);
    c1->cardinality = 100.0;
    c1->quality = 1.0;
    kb_save[1] = c1;

    // Create a derived concept
    OrdinalConcept *c2 = calloc(1, sizeof(OrdinalConcept));
    c2->id = 2;
    strncpy(c2->name, "biomass", MAX_NAME_LEN-1);
    c2->parent_A_id = 1;
    c2->cardinality = 100.0;
    c2->quality = 0.5;
    kb_save[2] = c2;

    int res = save_ontology_cbor(test_file, kb_save, 3);
    ASSERT_INT_EQ(res, 0);

    int load_size = 0;
    res = load_ontology_cbor(test_file, kb_load, &load_size);
    ASSERT_INT_EQ(res, 0);
    ASSERT_INT_EQ(load_size, 3);

    ASSERT_NOT_NULL(kb_load[1]);
    ASSERT_STR_EQ(kb_load[1]->name, "solar energy");
    ASSERT_INT_EQ(kb_load[1]->parent_A_id, 0);
    ASSERT_NULL(kb_load[1]->parent_A);

    ASSERT_NOT_NULL(kb_load[2]);
    ASSERT_STR_EQ(kb_load[2]->name, "biomass");
    ASSERT_INT_EQ(kb_load[2]->parent_A_id, 1);

    // Pass 2 Resolution Check
    ASSERT_NOT_NULL(kb_load[2]->parent_A);
    ASSERT_INT_EQ(kb_load[2]->parent_A->id, 1);
    ASSERT_STR_EQ(kb_load[2]->parent_A->name, "solar energy");

    // Cleanup
    unlink(test_file);
    for(int i=0; i<MAX_CONCEPTS; i++) {
        if(kb_save[i]) free(kb_save[i]);
        if(kb_load[i]) free(kb_load[i]);
    }
}

void run_memory_tests(void) {
    RUN_TEST(test_memory_roundtrip);
}

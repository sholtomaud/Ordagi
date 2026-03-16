#include "ordinal_tensor.h"
#include <cbor.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
 * CBOR Record Schema (FRD 5.2):
 * {
 *   "id":                 uint32,
 *   "name":               text string,
 *   "aliases":            array of text strings,
 *   "cardinality":        float64,
 *   "quality":            float64,
 *   "ordinal_branch_pos": float64,
 *   "ordinal_branch_neg": float64,
 *   "ordinal_resolved":   uint8,
 *   "ordinal_depth":      uint32,
 *   "parent_A_id":        uint32,
 *   "parent_B_id":        uint32,
 *   "interaction_type":   text string,
 *   "alpha":              float64,
 *   "verb":               text string,
 *   "adverb":             text string
 * }
 */

static cbor_item_t* concept_to_cbor(OrdinalConcept *c) {
    cbor_item_t *root = cbor_new_definite_map(15);
    bool ok = true;

    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("id")),
        .value = cbor_move(cbor_build_uint64(c->id))
    });
    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("name")),
        .value = cbor_move(cbor_build_string(c->name))
    });

    cbor_item_t *aliases = cbor_new_definite_array(c->alias_count);
    for (int i = 0; i < c->alias_count; i++) {
        ok &= cbor_array_set(aliases, i, cbor_move(cbor_build_string(c->aliases[i])));
    }
    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("aliases")),
        .value = cbor_move(aliases)
    });

    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("cardinality")),
        .value = cbor_move(cbor_build_float8(c->cardinality))
    });
    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("quality")),
        .value = cbor_move(cbor_build_float8(c->quality))
    });
    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("ordinal_branch_pos")),
        .value = cbor_move(cbor_build_float8(c->ordinal_value.branch_pos))
    });
    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("ordinal_branch_neg")),
        .value = cbor_move(cbor_build_float8(c->ordinal_value.branch_neg))
    });
    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("ordinal_resolved")),
        .value = cbor_move(cbor_build_uint64(c->ordinal_value.is_resolved))
    });
    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("ordinal_depth")),
        .value = cbor_move(cbor_build_uint64(c->ordinal_depth))
    });
    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("parent_A_id")),
        .value = cbor_move(cbor_build_uint64(c->parent_A_id))
    });
    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("parent_B_id")),
        .value = cbor_move(cbor_build_uint64(c->parent_B_id))
    });
    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("interaction_type")),
        .value = cbor_move(cbor_build_string(c->interaction_type))
    });
    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("alpha")),
        .value = cbor_move(cbor_build_float8(c->alpha))
    });
    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("verb")),
        .value = cbor_move(cbor_build_string(c->verb))
    });
    ok &= cbor_map_add(root, (struct cbor_pair) {
        .key = cbor_move(cbor_build_string("adverb")),
        .value = cbor_move(cbor_build_string(c->adverb))
    });

    (void)ok;
    return root;
}

int save_ontology_cbor(const char *filepath, OrdinalConcept **kb, int kb_size) {
    FILE *f = fopen(filepath, "wb");
    if (!f) return -1;

    for (int i = 0; i < kb_size; i++) {
        if (!kb[i]) continue;
        cbor_item_t *item = concept_to_cbor(kb[i]);
        unsigned char *buffer;
        size_t buffer_size, length;
        length = cbor_serialize_alloc(item, &buffer, &buffer_size);
        fwrite(buffer, 1, length, f);
        free(buffer);
        cbor_decref(&item);
    }

    fclose(f);
    return 0;
}

int append_concept_to_cbor(const char *filepath, OrdinalConcept *c) {
    FILE *f = fopen(filepath, "ab");
    if (!f) return -1;

    cbor_item_t *item = concept_to_cbor(c);
    unsigned char *buffer;
    size_t buffer_size, length;
    length = cbor_serialize_alloc(item, &buffer, &buffer_size);
    fwrite(buffer, 1, length, f);
    free(buffer);
    cbor_decref(&item);

    fclose(f);
    return 0;
}

int load_ontology_cbor(const char *filepath, OrdinalConcept **kb, int *kb_size) {
    FILE *f = fopen(filepath, "rb");
    if (!f) return -1;

    fseek(f, 0, SEEK_END);
    size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char *buffer = malloc(fsize);
    if (!buffer) { fclose(f); return -1; }
    fread(buffer, 1, fsize, f);
    fclose(f);

    struct cbor_load_result result;
    size_t offset = 0;
    int count = 0;

    // Pass 1: Load records
    while (offset < fsize) {
        cbor_item_t *item = cbor_load(buffer + offset, fsize - offset, &result);
        if (!item) break;
        offset += result.read;

        if (cbor_isa_map(item)) {
            OrdinalConcept *c = calloc(1, sizeof(OrdinalConcept));
            if (!c) {
                fprintf(stderr, "[FATAL] out of memory in load_ontology_cbor\n");
                cbor_decref(&item);
                break;
            }
            struct cbor_pair *pairs = cbor_map_handle(item);
            size_t size = cbor_map_size(item);

            for (size_t i = 0; i < size; i++) {
                size_t klen = cbor_string_length(pairs[i].key);
                char key[64];
                if (klen >= 64) klen = 63;
                memcpy(key, cbor_string_handle(pairs[i].key), klen);
                key[klen] = '\0';

                cbor_item_t *val = pairs[i].value;

                if (strcmp(key, "id") == 0) c->id = (int)cbor_get_int(val);
                else if (strcmp(key, "name") == 0) {
                    size_t vlen = cbor_string_length(val);
                    if (vlen >= MAX_NAME_LEN) vlen = MAX_NAME_LEN - 1;
                    memcpy(c->name, cbor_string_handle(val), vlen);
                    c->name[vlen] = '\0';
                }
                else if (strcmp(key, "cardinality") == 0) c->cardinality = cbor_float_get_float8(val);
                else if (strcmp(key, "quality") == 0) c->quality = cbor_float_get_float8(val);
                else if (strcmp(key, "ordinal_branch_pos") == 0) c->ordinal_value.branch_pos = cbor_float_get_float8(val);
                else if (strcmp(key, "ordinal_branch_neg") == 0) c->ordinal_value.branch_neg = cbor_float_get_float8(val);
                else if (strcmp(key, "ordinal_resolved") == 0) c->ordinal_value.is_resolved = (int)cbor_get_int(val);
                else if (strcmp(key, "ordinal_depth") == 0) c->ordinal_depth = (int)cbor_get_int(val);
                else if (strcmp(key, "parent_A_id") == 0) c->parent_A_id = (int)cbor_get_int(val);
                else if (strcmp(key, "parent_B_id") == 0) c->parent_B_id = (int)cbor_get_int(val);
                else if (strcmp(key, "interaction_type") == 0) {
                    size_t vlen = cbor_string_length(val);
                    if (vlen >= 32) vlen = 31;
                    memcpy(c->interaction_type, cbor_string_handle(val), vlen);
                    c->interaction_type[vlen] = '\0';
                }
                else if (strcmp(key, "alpha") == 0) c->alpha = cbor_float_get_float8(val);
                else if (strcmp(key, "verb") == 0) {
                    size_t vlen = cbor_string_length(val);
                    if (vlen >= 32) vlen = 31;
                    memcpy(c->verb, cbor_string_handle(val), vlen);
                    c->verb[vlen] = '\0';
                }
                else if (strcmp(key, "adverb") == 0) {
                    size_t vlen = cbor_string_length(val);
                    if (vlen >= 32) vlen = 31;
                    memcpy(c->adverb, cbor_string_handle(val), vlen);
                    c->adverb[vlen] = '\0';
                }
                else if (strcmp(key, "aliases") == 0) {
                    c->alias_count = (int)cbor_array_size(val);
                    for (int j = 0; j < c->alias_count && j < MAX_ALIASES; j++) {
                        cbor_item_t *alias_item = cbor_array_get(val, j);
                        if (cbor_isa_string(alias_item)) {
                            size_t vlen = cbor_string_length(alias_item);
                            if (vlen >= MAX_ALIAS_LEN) vlen = MAX_ALIAS_LEN - 1;
                            memcpy(c->aliases[j], cbor_string_handle(alias_item), vlen);
                            c->aliases[j][vlen] = '\0';
                        }
                    }
                }
            }
            if (c->id >= 0 && c->id < MAX_CONCEPTS) {
                kb[c->id] = c;
                if (c->id >= count) count = c->id + 1;
            } else {
                fprintf(stderr, "[ERROR] concept ID %d out of bounds (MAX: %d)\n", c->id, MAX_CONCEPTS);
                free(c);
            }
        }
        cbor_decref(&item);
    }
    free(buffer);
    *kb_size = count;

    // Pass 2: Resolve pointers
    for (int i = 0; i < *kb_size; i++) {
        if (kb[i]) {
            if (kb[i]->parent_A_id > 0 && kb[i]->parent_A_id < MAX_CONCEPTS) {
                kb[i]->parent_A = kb[kb[i]->parent_A_id];
            }
            if (kb[i]->parent_B_id > 0 && kb[i]->parent_B_id < MAX_CONCEPTS) {
                kb[i]->parent_B = kb[kb[i]->parent_B_id];
            }
        }
    }

    return 0;
}

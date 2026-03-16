#ifndef VERB_TABLE_H
#define VERB_TABLE_H

#include <stddef.h>

typedef struct {
    const char *verb;
    const char *interaction_type;
    double      alpha;
    const char *default_adverb;
} OdumVerbEntry;

static const OdumVerbEntry ODUM_VERB_TABLE[] = {

    // --- CO-PRODUCTION (alpha=0.5, q=1/2): binary function output ---
    // Both inputs necessary; output inherits form of both parents
    { "combines",      "co-production", 0.5, "jointly"        },
    { "transforms",    "co-production", 0.5, "efficiently"    },
    { "converts",      "co-production", 0.5, "directly"       },
    { "produces",      "co-production", 0.5, "continuously"   },
    { "generates",     "co-production", 0.5, "actively"       },
    { "synthesises",   "co-production", 0.5, "chemically"     },
    { "synthesizes",   "co-production", 0.5, "chemically"     },
    { "yields",        "co-production", 0.5, "naturally"      },
    { "catalyses",     "co-production", 0.5, "rapidly"        },
    { "catalyzes",     "co-production", 0.5, "rapidly"        },
    { "processes",     "co-production", 0.5, "systematically" },
    { "metabolises",   "co-production", 0.5, "biologically"   },
    { "metabolizes",   "co-production", 0.5, "biologically"   },
    { "constructs",    "co-production", 0.5, "structurally"   },
    { "builds",        "co-production", 0.5, "incrementally"  },
    { "assembles",     "co-production", 0.5, "precisely"      },
    { "integrates",    "co-production", 0.5, "coherently"     },

    // --- INTER-ACTION (alpha=2.0, q=2): duet function output ---
    // Output is a new entity of superior hierarchical order; models
    // Riccati-type self-organisation (Giannantoni 2006, Eq. 3.16–3.20).
    // Added in v3.0 from Giannantoni primary sources.
    { "interacts",      "inter-action",  2.0, "mutually"       },
    { "couples",        "inter-action",  2.0, "tightly"        },
    { "entangles",      "inter-action",  2.0, "indissolubly"   },
    { "organises",      "inter-action",  2.0, "self-"          },
    { "organizes",      "inter-action",  2.0, "self-"          },
    { "emerges",        "inter-action",  2.0, "spontaneously"  },
    { "co-evolves",     "inter-action",  2.0, "reciprocally"   },
    { "folds",          "inter-action",  2.0, "hierarchically" },
    { "complexifies",   "inter-action",  2.0, "progressively"  },
    { "differentiates", "inter-action",  2.0, "structurally"   },

    // --- FEED-BACK (alpha=1.0, q=2/2): binary-duet function output ---
    // Output reinforces its own generative inputs
    { "drives",        "feed-back",     1.0, "forcefully"     },
    { "amplifies",     "feed-back",     1.0, "strongly"       },
    { "accelerates",   "feed-back",     1.0, "rapidly"        },
    { "reinforces",    "feed-back",     1.0, "positively"     },
    { "regulates",     "feed-back",     1.0, "precisely"      },
    { "controls",      "feed-back",     1.0, "tightly"        },
    { "maintains",     "feed-back",     1.0, "steadily"       },
    { "sustains",      "feed-back",     1.0, "continuously"   },
    { "supports",      "feed-back",     1.0, "directly"       },
    { "enables",       "feed-back",     1.0, "functionally"   },
    { "modulates",     "feed-back",     1.0, "dynamically"    },
    { "stabilises",    "feed-back",     1.0, "homeostatically"},
    { "stabilizes",    "feed-back",     1.0, "homeostatically"},

    // --- STORAGE (alpha=1.0): treated as feed-back in Odum algebra ---
    { "stores",        "feed-back",     1.0, "stably"         },
    { "accumulates",   "feed-back",     1.0, "gradually"      },
    { "retains",       "feed-back",     1.0, "persistently"   },
    { "absorbs",       "feed-back",     1.0, "passively"      },
    { "captures",      "feed-back",     1.0, "selectively"    },
    { "concentrates",  "feed-back",     1.0, "locally"        },

    // --- DISSIPATION (alpha=1.0): energy drain, no emergent structure ---
    { "consumes",      "dissipation",   1.0, "irreversibly"   },
    { "degrades",      "dissipation",   1.0, "thermally"      },
    { "dissipates",    "dissipation",   1.0, "gradually"      },
    { "loses",         "dissipation",   1.0, "inevitably"     },
    { "releases",      "dissipation",   1.0, "spontaneously"  },
    { "depletes",      "dissipation",   1.0, "progressively"  },
    { "exhausts",      "dissipation",   1.0, "irreversibly"   },

    // --- SENTINEL ---
    { NULL, NULL, 0.0, NULL }
};

#endif /* VERB_TABLE_H */

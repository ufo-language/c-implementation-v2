#include "main/setup.h"

void globals_permanentObjects(void);
void globals_rootObjects(void);
void parser_permanentObjects(void);

void lexer_rootObjects(void);
void ns_all_rootObjects(void);
void parser_rootObjects(void);
void threadManager_rootObjects(void);

void lexer_initialize(void);

// Called by gc_start. These objects are never swept by the GC,
// whether or not they are ever marked.
void setup_permanentObjects(void) {
    globals_permanentObjects();
    parser_permanentObjects();
}

// Root objects are always marked during GC collection.
void setup_rootObjects(void) {
    globals_rootObjects();  // globals must be first
    lexer_rootObjects();
    parser_rootObjects();
    ns_all_rootObjects();
    threadManager_rootObjects();
}

// Called by gc_start after the permanent objects and root objects are
// set up.
void setup_initialize(void) {
    lexer_initialize();
}

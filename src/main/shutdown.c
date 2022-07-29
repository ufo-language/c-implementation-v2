void bucketLink_freeAll(void);

void shutdown(void) {
    bucketLink_freeAll();
}

cdef extern from "../libpngpack/pngpack.h":
    cdef struct pngpack_bounds:
        double min_w, max_w
        double min_h, max_h

    cdef struct pngpack_channel:
        pass

    cdef struct pngpack:
        pass

    pngpack* pngpack_new(size_t width, size_t height, pngpack_bounds bounds, char *text_namespace)
    void pngpack_free(pngpack *pp)

    void pngpack_add_textfield(pngpack *pp, char *key, char *value);
    void pngpack_add_channel(pngpack *pp, pngpack_channel *channel)
    bint pngpack_write(pngpack *pp, char *path)

    pngpack_channel* pngpack_channel_new(char *name, double *data, size_t data_length)
    void pngpack_channel_free(pngpack_channel *channel)

    void pngpack_channel_add_textfield(pngpack_channel *channel, char *key, char *value)

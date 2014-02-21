#ifndef LOAD_FILE_H
#define LOAD_FILE_H

void load_file_async(PP_Instance instance, const char *fname,
		     void (*callback)(char *, int, void *), void *callback_args);

#endif /* LOAD_FILE_H */

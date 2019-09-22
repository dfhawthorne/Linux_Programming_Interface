/* ------------------------------------------------------------------------------
 * Listing 3-2: Declarations for common error-handling functions
 * p.52, "Linux Progamming Interface"
 * --------------------------------------------------------------------------- */

#ifndef ERROR_FUNCTIONS_H
#define ERROR_FUNCTIONS_H          /* Prevent accidental double inclusion */

void errMsg(const char *format, ...);

#ifdef __GNUC__

/* This macro stops 'gcc -Wall' complaining that "control reaches
    end of non-void function" if we use the following functions to
    terminate main() or some other non-void function. */

#define NORETURN __attribute__ ((__noreturn__))
#else
#define NORETURN
#endif

void errExit(const char *format, ...) NORETURN;

void err_exit(const char *format, ...) NORETURN;

void errExitEN(int errnum, const char *format, ...) NORETURN;

void fatal(const char *format, ...) NORETURN;

void usageErr(const char *format, ...) NORETURN;

void cmdLineErr(const char *format, ...) NORETURN;

#endif
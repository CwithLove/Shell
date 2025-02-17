/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#ifndef __READCMD_H
#define __READCMD_H

/**
 * @brief Read a command line from standard input.
 * 
 * 
 * 
 */
struct cmdline *readcmd(void);


/* Structure returned by readcmd() */
struct cmdline {
	
	/**
	 * @brief Error message, if not null.
	 * 
	 * If this field is not null, it is an error message that should be displayed.
	 */
	char *err;

	/**
	 * @brief Name of the file for input redirection, if not null.
	 * 
	 * If this field is not null, it specifies the file name for input redirection.
	 */
	char *in;

	/**
	 * @brief Name of the file for output redirection, if not null.
	 * 
	 * If this field is not null, it specifies the file name for output 
	 * redirection.
	 */
	char *out;

	/**
	 * @brief Sequence of commands, if not null.
	 * 
	 * This field points to an array of commands. Each command is an array of strings, 
	 * and the last command is followed by a NULL pointer.
	 * 
	 * Read the comment at the end of the file for more details.
	 */
	char ***seq;

	/**
	 * @brief Background indicator.
	 * 
	 * If this field is not 0, it indicates that the command line should be executed
	 * in the background.
	 */
	int bg;
};
/**
 * @brief Description of the `seq` field of the `struct cmdline` structure. 
 * 
 * A command line is a sequence of commands whose output is linked to the input
 * of the next command by a pipe.
 * 
 * - A **command** is an array of strings (char **), whose last item is a null pointer.
 * 
 * - A **sequence** is an array of commands (char ***), whose last item is a null
 * pointer.
 * 
 * When a `cmdline` structure is returned by `readcmd()`, `seq[0]` is never null.
 */

#endif // __READCMD_H

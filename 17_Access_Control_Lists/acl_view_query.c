/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2019.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/*************************************************************************\
* Exercise 17.1:                                                          *
* Displays the permissions from the ACL entry that corresponds to a       *
* user or group.                                                          *
\*************************************************************************/

#include <acl/libacl.h>
#include <sys/acl.h>
#include "ugid_functions.h"
#include "tlpi_hdr.h"
#include <ctype.h>

static void
usageError(char *progName)
{
    fprintf(stderr, "Usage: %s (-u (user|uid) | -g (group|gid)) [-d] filename\n", progName);
    exit(EXIT_FAILURE);
}

/*
 * Converts an ACL permission set into a bit-string (rwx)
 */

int
getPerm(acl_permset_t permset)
{
    int perms = 0;
    int permVal = 0;
    
    permVal = acl_get_perm(permset, ACL_READ);
    if (permVal == -1)
        errExit("acl_get_perm - ACL_READ");
    perms |= (permVal == 1) ? 4 : 0;
    permVal = acl_get_perm(permset, ACL_WRITE);
    if (permVal == -1)
        errExit("acl_get_perm - ACL_WRITE");
    perms |= (permVal == 1) ? 2 : 0;
    permVal = acl_get_perm(permset, ACL_EXECUTE);
    if (permVal == -1)
        errExit("acl_get_perm - ACL_EXECUTE");
    perms |= (permVal == 1) ? 1 : 0;
    
    return perms;
}

int
main(int argc, char *argv[])
{
    acl_t acl;
    acl_type_t type;
    acl_entry_t entry;
    acl_tag_t tag;
    uid_t *uidp;
    gid_t *gidp;
    acl_permset_t permset;
    char *name;
    int entryId, opt;
    char *groupStr = NULL,
        *userStr   = NULL,
        *entryType = NULL;
    char entryName[10];
    int uid = -1;
    int gid = -1;
    int foundPerms = 0;
    int maskPerms  = 0;
    short verbosity = 0;

    type = ACL_TYPE_ACCESS;
    while ((opt = getopt(argc, argv, "u:g:dv")) != -1) {
        switch (opt) {
        case 'd': type = ACL_TYPE_DEFAULT;      break;
        case 'g': groupStr = optarg;            break;
        case 'u': userStr  = optarg;            break;
        case 'v': verbosity++;                  break;
        case '?': usageError(argv[0]);
        }
    }
    
    if (optind + 1 != argc)
        usageError(argv[0]);
    
    /* Ensure that only one of user or group is specfied */

    if (groupStr != NULL) {
        if (verbosity > 1)
            printf("Group=%s\n", groupStr);
        if (userStr != NULL) {
            if (verbosity > 1)
                printf("User=%s\n", userStr);
            errExit("Specify only one of either user or group");
        } else {
            if (isdigit(groupStr[0])) {
                gid = atoi(groupStr);
            } else {
                gid = groupIdFromName(groupStr);
            }
        }
    } else {
        if (userStr == NULL) {
            errExit("Specify either user or group");
        } else {
            if (verbosity > 1)
                printf("User=%s\n", userStr);
            if (isdigit(userStr[0])) {
                uid = atoi(userStr);
            } else {
                uid = userIdFromName(userStr);
            }
        }
    }        

    acl = acl_get_file(argv[optind], type);
    if (acl == NULL)
        errExit("acl_get_file");

    /* Walk through each entry in this ACL */

    for (entryId = ACL_FIRST_ENTRY; ; entryId = ACL_NEXT_ENTRY) {

        if (acl_get_entry(acl, entryId, &entry) != 1)
            break;                      /* Exit on error or no more entries */

        /* Retrieve tag type */

        if (acl_get_tag_type(entry, &tag) == -1)
            errExit("acl_get_tag_type");
        
        /* Retrieve the associated permissions set */ 
        
        if (acl_get_permset(entry, &permset) == -1)
            errExit("acl_get_permset");

        /*
         * If there is a matching ACL entry (user or group), record the details
         * (entry type and name), and save the decoded permissions set for later.
         * Also, save the permissions set mask for later.
         */

        if (tag == ACL_USER) {
            uidp = acl_get_qualifier(entry);
            if (uidp == NULL)
                errExit("acl_get_qualifier");

            if (*uidp == uid) {
                entryType = "User";
                name = userNameFromId(*uidp);
                if (name == NULL)
                    sprintf(entryName, "%-8d ", *uidp);
                else
                    sprintf(entryName, "%-8s ", name);
                foundPerms = getPerm(permset);
                if (verbosity > 1)
                    printf("User perms found=%d\n", foundPerms);
            }

            if (acl_free(uidp) == -1)
                errExit("acl_free");

        } else if (tag == ACL_GROUP) {
            gidp = acl_get_qualifier(entry);
            if (gidp == NULL)
                errExit("acl_get_qualifier");

            if (*gidp == gid) {
                entryType = "Group";
                name = groupNameFromId(*gidp);
                if (name == NULL)
                    sprintf(entryName, "%-8d ", *gidp);
                else
                    sprintf(entryName, "%-8s ", name);
                foundPerms = getPerm(permset);
                if (verbosity > 1)
                    printf("Group perms found=%d\n", foundPerms);
            }

            if (acl_free(gidp) == -1)
                errExit("acl_free");

        } else if (tag == ACL_MASK) {
            maskPerms = getPerm(permset);
            if (verbosity > 1)
                printf("Mask perms found=%d\n", maskPerms);
        }
    }

    if (acl_free(acl) == -1)
        errExit("acl_free");

    /*
     * If we found a matching entry (user or group), print out the allowed
     * permissions.
     */
    
    if (entryName != NULL) {
        int actualPerms = foundPerms & maskPerms;
        if (verbosity > 1)
            printf("Actual perms calculated=%d\n", actualPerms);
        printf("%-12s", entryType);
        printf("%-9s", entryName);
        printf("%c", ((actualPerms & 4) == 4) ? 'r' : '-');
        printf("%c", ((actualPerms & 2) == 2) ? 'w' : '-');
        printf("%c", ((actualPerms & 1) == 1) ? 'x' : '-');

        printf("\n");
        exit(EXIT_SUCCESS);
    } else {
        exit(EXIT_FAILURE);
    }
}

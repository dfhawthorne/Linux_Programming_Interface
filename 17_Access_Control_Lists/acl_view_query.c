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
    int entryId, permVal, opt;
    char *groupStr = NULL,
        *userStr = NULL;
    int uid = -1;
    int gid = -1;

    type = ACL_TYPE_ACCESS;
    while ((opt = getopt(argc, argv, "u:g:d")) != -1) {
        switch (opt) {
        case 'd': type = ACL_TYPE_DEFAULT;      break;
        case 'g': groupStr = optarg;            break;
        case 'u': userStr  = optarg;            break;
        case '?': usageError(argv[0]);
        }
    }
    
    if (optind + 1 != argc)
        usageError(argv[0]);
    
    /* Ensure that only one of user or group is specfied */

    if (groupStr != NULL) {
        /* printf("Group=%s\n", groupStr); */
        if (userStr != NULL) {
            /* printf("User=%s\n", userStr); */
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
            /* printf("User=%s\n", userStr); */
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
        Boolean dispPerms;
        dispPerms = FALSE;

        if (acl_get_entry(acl, entryId, &entry) != 1)
            break;                      /* Exit on error or no more entries */

        /* Retrieve and display tag type */

        if (acl_get_tag_type(entry, &tag) == -1)
            errExit("acl_get_tag_type");

        /* Retrieve and display optional tag qualifier */

        if (tag == ACL_USER) {
            uidp = acl_get_qualifier(entry);
            if (uidp == NULL)
                errExit("acl_get_qualifier");

            if (*uidp == uid) {
                printf("%-12s", "User");
                name = userNameFromId(*uidp);
                if (name == NULL)
                    printf("%-8d ", *uidp);
                else
                    printf("%-8s ", name);
                dispPerms = TRUE;
            }

            if (acl_free(uidp) == -1)
                errExit("acl_free");

        } else if (tag == ACL_GROUP) {
            gidp = acl_get_qualifier(entry);
            if (gidp == NULL)
                errExit("acl_get_qualifier");

            if (*gidp == gid) {
                printf("%-12s", "Group");
                name = groupNameFromId(*gidp);
                if (name == NULL)
                    printf("%-8d ", *gidp);
                else
                    printf("%-8s ", name);
                dispPerms = TRUE;
            }

            if (acl_free(gidp) == -1)
                errExit("acl_free");

        } else {
            continue;
        }

        /* Retrieve and display permissions */

        /* printf("Tag=%d, dispPerms=%d, uid=%d, gid=%d\n", tag, dispPerms, uid, gid); */
        
        if (acl_get_permset(entry, &permset) == -1)
            errExit("acl_get_permset");

        if (dispPerms) {
            permVal = acl_get_perm(permset, ACL_READ);
            if (permVal == -1)
                errExit("acl_get_perm - ACL_READ");
            printf("%c", (permVal == 1) ? 'r' : '-');
            permVal = acl_get_perm(permset, ACL_WRITE);
            if (permVal == -1)
                errExit("acl_get_perm - ACL_WRITE");
            printf("%c", (permVal == 1) ? 'w' : '-');
            permVal = acl_get_perm(permset, ACL_EXECUTE);
            if (permVal == -1)
                errExit("acl_get_perm - ACL_EXECUTE");
            printf("%c", (permVal == 1) ? 'x' : '-');

            printf("\n");
        }
    }

    if (acl_free(acl) == -1)
        errExit("acl_free");

    exit(EXIT_SUCCESS);
}

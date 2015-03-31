/*
 * Device Mapper PMDA - Thin Provisioning (dm-thin) Stats
 *
 * Copyright (c) 2015 Red Hat.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

#include "pmapi.h"
#include "impl.h"
#include "pmda.h"

#include "dmthin.h"

#include <inttypes.h>

/*
 * Fetches the value for the given metric item and then assigns to pmAtomValue.
 * We check to see if item is in valid range for the metric.
 */
int
dm_thin_pool_fetch(int item, struct pool_stats *pool_stats, pmAtomValue *atom)
{
    if (item < 0 || item >= NUM_POOL_STATS)
	return PM_ERR_PMID;

    switch(item) {
        case POOL_SIZE:
            atom->ull = pool_stats->size;
            break;
        case POOL_TRANS_ID:
            atom->ull = pool_stats->trans_id;
            break;
        case POOL_META_USED:
            atom->ull = pool_stats->meta_used;
            break;
        case POOL_META_TOTAL:
            atom->ull = pool_stats->meta_total;
            break;
        case POOL_DATA_USED:
            atom->ull = pool_stats->data_used;
            break;
        case POOL_DATA_TOTAL:
            atom->ull = pool_stats->data_total;
            break;
        case POOL_HELD_ROOT:
            atom->cp = pool_stats->held_root;
            break;
        case POOL_READ_MODE:
            atom->cp = pool_stats->read_mode;
            break;
        case POOL_DISCARD_PASSDOWN:
            atom->cp = pool_stats->discard_passdown;
            break;
        case POOL_NO_SPACE_MODE:
            atom->cp = pool_stats->no_space_mode;
            break;
    }     
    return 1;
}

/*
 * Fetches the value for the given metric item and then assigns to pmAtomValue.
 * We check to see if item is in valid range for the metric.
 */
int
dm_thin_vol_fetch(int item, struct vol_stats *vol_stats, pmAtomValue *atom)
{
    if (item < 0 || item >= NUM_VOL_STATS)
	return PM_ERR_PMID;

    switch(item) {
        case VOL_SIZE:
            atom->ull = vol_stats->size;
            break;
        case VOL_NUM_MAPPED_SECTORS:
            atom->ull = vol_stats->num_mapped_sectors;
            break;
        case VOL_HIGHEST_MAPPED_SECTORS:
            atom->ull = vol_stats->high_mapped_sector;
            break;
    }     
    return 1;
}

/* 
 * Grab output from dmsetup status (or read in from cat when under QA),
 * Match the data to the pool which we wish to update the metrics and
 * assign the values to pool_stats. 
 */
int
dm_refresh_thin_pool(const int _isQA, const char *dm_statspath, const char *pool_name, struct pool_stats *pool_stats){
    char buffer[PATH_MAX] = "dmsetup status --target thin-pool";
    char *token;
    uint64_t size_start, size_end;
    FILE *fp;

    if (_isQA) {
        snprintf(buffer, sizeof(buffer),"/bin/cat %s/dmthin-pool", dm_statspath);
        buffer[sizeof(buffer)-1] = '\0';
    }

    if ((fp = popen(buffer, "r")) == NULL )
        return - oserror();

    while (fgets(buffer, sizeof(buffer) -1, fp)) {
        if (!strstr(buffer, ":") || strstr(buffer, "Fail"))
            continue;

        token = strtok(buffer, ":");

        if (strcmp(token, pool_name) == 0) {
            token = strtok(NULL, ":");

            /* Pattern match our output to the given thin-pool status
             * output (minus pool name). 
             * The format is:
             * <name>:<start> <end> <target>
             *     <transaction id> <used metadata blocks>/<total metadata blocks>
             *     <used data blocks>/<total data blocks> <held metadata root>
             *     ro|rw [no_]discard_passdown  [error|queue]_if_no_space
             */
            sscanf(token, " %"SCNu64" %"SCNu64" thin-pool %"SCNu64" %"SCNu64"/%"SCNu64" %"SCNu64"/%"SCNu64" %s %s %s %s",
                &size_start,
                &size_end,
                &pool_stats->trans_id,
                &pool_stats->meta_used,
                &pool_stats->meta_total,
                &pool_stats->data_used,
                &pool_stats->data_total,
                pool_stats->held_root,
                pool_stats->read_mode,
                pool_stats->discard_passdown,
                pool_stats->no_space_mode
            );
            pool_stats->size = (size_end - size_start);
        }
    }

    if (pclose(fp) != 0)
        return -oserror(); 

    return 0;
}

/* 
 * Grab output from dmsetup status (or read in from cat when under QA),
 * Match the data to the volume which we wish to update the metrics and
 * assign the values to vol_stats. 
 */
int
dm_refresh_thin_vol(const int _isQA, const char *dm_statspath, const char *vol_name, struct vol_stats *vol_stats){
    char buffer[PATH_MAX] = "dmsetup status --target thin";
    char *token;
    uint64_t size_start, size_end;
    FILE *fp;

    if (_isQA) {
        snprintf(buffer, sizeof(buffer),"/bin/cat %s/dmthin-thin", dm_statspath);
        buffer[sizeof(buffer)-1] = '\0';
    }

    if ((fp = popen(buffer, "r")) == NULL )
        return - oserror();

    while (fgets(buffer, sizeof(buffer) -1, fp)) {
        if (!strstr(buffer, ":") || strstr(buffer, "Fail"))
            continue;

        token = strtok(buffer, ":");

        if (strcmp(token, vol_name) == 0) {
            token = strtok(NULL, ":");

            /* Pattern match our output to the given thin-volume status
             * output (minus volume name). 
             * The format is:
             * <name>:<start> <end> <target>
             *     <nr mapped sectors> <highest mapped sector>
             */
            sscanf(token, " %"SCNu64" %"SCNu64" thin %"SCNu64" %"SCNu64"",
                &size_start,
                &size_end,
                &vol_stats->num_mapped_sectors,
                &vol_stats->high_mapped_sector
            );
            vol_stats->size = (size_end - size_start);
        }
    }

    if (pclose(fp) != 0)
        return -oserror(); 

    return 0;
}

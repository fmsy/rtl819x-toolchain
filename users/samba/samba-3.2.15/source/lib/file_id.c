/* 
   Unix SMB/CIFS implementation.

   file_id structure handling

   Copyright (C) Andrew Tridgell 2007
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "includes.h"

/*
  return a file_id which gives a unique ID for a file given the device and
  inode numbers
 */
struct file_id file_id_create_dev(SMB_DEV_T dev, SMB_INO_T inode)
{
	struct file_id key;
	/* the ZERO_STRUCT ensures padding doesn't break using the key as a
	 * blob */
	ZERO_STRUCT(key);
	key.devid = dev;
	key.inode = inode;
	return key;
}

/*
  generate a file_id from a stat structure
 */
struct file_id vfs_file_id_from_sbuf(connection_struct *conn, const SMB_STRUCT_STAT *sbuf)
{
	return SMB_VFS_FILE_ID_CREATE(conn, sbuf->st_dev, sbuf->st_ino);
}

/*
  return True if two file_id structures are equal
 */
bool file_id_equal(const struct file_id *id1, const struct file_id *id2)
{
	return id1->inode == id2->inode && id1->devid == id2->devid;
}

/*
  a static string for a file_id structure
 */
const char *file_id_string_tos(const struct file_id *id)
{
	char *result = talloc_asprintf(talloc_tos(), "%llx:%llx", 
				       (unsigned long long)id->devid, 
				       (unsigned long long)id->inode);
	SMB_ASSERT(result != NULL);
	return result;
}

/*
  push a 16 byte version of a file id into a buffer
 */
void push_file_id_16(char *buf, const struct file_id *id)
{
	SIVAL(buf,  0, id->devid&0xFFFFFFFF);
	SIVAL(buf,  4, id->devid>>32);
	SIVAL(buf,  8, id->inode&0xFFFFFFFF);
	SIVAL(buf, 12, id->inode>>32);
}

/*
  pul a 16 byte version of a file id from a buffer
 */
void pull_file_id_16(char *buf, struct file_id *id)
{
	ZERO_STRUCTP(id);
	id->devid  = IVAL(buf,  0);
	id->devid |= ((uint64_t)IVAL(buf,4))<<32;
	id->inode  = IVAL(buf,  8);
	id->inode |= ((uint64_t)IVAL(buf,12))<<32;
}

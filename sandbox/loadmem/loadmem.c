/* This file is part of sp-stress
 *
 * Copyright (C) 2009 Nokia Corporation. 
 *
 * Contact: Jean-Luc Lamadon <jean-luc.lamadon@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, const char* argv[])
{
   if (2 == argc)
   {
      const unsigned size = strtoul(argv[1], NULL, 0) << 20;
      const unsigned block_size = 512 << 10;
      int big_chunk = size - size * 0.05;

      int i = 1, block_nbr = (size - big_chunk) / block_size;
      void* data;
      printf("%d\n",block_nbr);
      printf("%d\n",big_chunk);
      printf("%d\n",block_size);

      data = malloc (block_size + big_chunk);
      for (i = 1 ; i < block_nbr ; i++)
      {
          data = (char*) realloc (data, i * block_size + big_chunk);
          memset(data, '-', i * block_size + big_chunk);
      }
      printf ("loadmem eats %u MB \n", (i * block_size + big_chunk) >> 20);

      while (1)
          sleep(5);
   }
   else
   {
       printf ("Usage: loadmem <Memory allocation in MB>\n");
       printf ("Example: loadmem 100\n");
   }
   return 0;
} /* main */

/*  WQSG_Lib
*  Copyright (C) WQSG <wqsg@qq.com>
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
*/
#ifdef __TEST_DEBUG__
#include<WQSG.h>
#include "WQSG_CxFile.h"


#include<malloc.h>
int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _AMD64_
	int h = 0;
#endif
	CWQSG_CmemFILE fp;

	char ddd[100];

	fp.fopen( ddd , sizeof(ddd) , true );
	return 0;
}


#endif
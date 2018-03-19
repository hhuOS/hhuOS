#include "FatFs.h"
#include <cstdarg>

extern "C" {
#include "lib/libc/string.h"
}

/*  CP437(U.S.) to Unicode conversion table */

const WCHAR FatFs::uc437[] = {
	0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7, 0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,
	0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9, 0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192,
	0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA, 0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
	0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556, 0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
	0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F, 0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
	0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B, 0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
	0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x00B5, 0x03C4, 0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229,
	0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248, 0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0
};

/* FAT: Offset of LFN characters in the directory entry */

const BYTE FatFs::LfnOfs[] = {1, 3, 5, 7, 9, 14, 16, 18, 20, 22, 24, 28, 30};


const BYTE FatFs::ExCvt[] = MKCVTBL(TBL_CT, FF_CODE_PAGE);

/* Unicode conversion functions */

WCHAR FatFs::ff_uni2oem(DWORD uni, WORD cp) {
    WCHAR c = 0;
	const WCHAR *p = CVTBL(uc, FF_CODE_PAGE);

	if (uni < 0x80) {	/* ASCII? */
		c = (WCHAR)uni;

	} else {			/* Non-ASCII */
		if (uni < 0x10000 && cp == FF_CODE_PAGE) {	/* Is it a valid code? */
			for (c = 0; c < 0x80 && uni != p[c]; c++) ;
			c = (c + 0x80) & 0xFF;
		}
	}

	return c;
}

WCHAR FatFs::ff_oem2uni(WCHAR oem, WORD cp) {
    WCHAR c = 0;
	const WCHAR *p = CVTBL(uc, FF_CODE_PAGE);

	if (oem < 0x80) {	/* ASCII? */
		c = oem;

	} else {			/* Extended char */
		if (cp == FF_CODE_PAGE) {	/* Is it a valid code page? */
			if (oem < 0x100) c = p[oem - 0x80];
		}
	}

	return c;
}

DWORD FatFs::ff_wtoupper(DWORD uni) {
    /* Compressed upper conversion table */
	static const WORD cvt1[] = {	/* U+0000 - U+0FFF */
		/* Basic Latin */
		0x0061,0x031A,
		/* Latin-1 Supplement */
		0x00E0,0x0317,  0x00F8,0x0307,  0x00FF,0x0001,0x0178,
		/* Latin Extended-A */
		0x0100,0x0130,  0x0132,0x0106,  0x0139,0x0110,  0x014A,0x012E,  0x0179,0x0106,
		/* Latin Extended-B */
		0x0180,0x004D,0x0243,0x0181,0x0182,0x0182,0x0184,0x0184,0x0186,0x0187,0x0187,0x0189,0x018A,0x018B,0x018B,0x018D,0x018E,0x018F,0x0190,0x0191,0x0191,0x0193,0x0194,0x01F6,0x0196,0x0197,0x0198,0x0198,0x023D,0x019B,0x019C,0x019D,0x0220,0x019F,0x01A0,0x01A0,0x01A2,0x01A2,0x01A4,0x01A4,0x01A6,0x01A7,0x01A7,0x01A9,0x01AA,0x01AB,0x01AC,0x01AC,0x01AE,0x01AF,0x01AF,0x01B1,0x01B2,0x01B3,0x01B3,0x01B5,0x01B5,0x01B7,0x01B8,0x01B8,0x01BA,0x01BB,0x01BC,0x01BC,0x01BE,0x01F7,0x01C0,0x01C1,0x01C2,0x01C3,0x01C4,0x01C5,0x01C4,0x01C7,0x01C8,0x01C7,0x01CA,0x01CB,0x01CA,
		0x01CD,0x0110,  0x01DD,0x0001,0x018E,  0x01DE,0x0112,  0x01F3,0x0003,0x01F1,0x01F4,0x01F4,  0x01F8,0x0128,
		0x0222,0x0112,  0x023A,0x0009,0x2C65,0x023B,0x023B,0x023D,0x2C66,0x023F,0x0240,0x0241,0x0241,  0x0246,0x010A,
		/* IPA Extensions */
		0x0253,0x0040,0x0181,0x0186,0x0255,0x0189,0x018A,0x0258,0x018F,0x025A,0x0190,0x025C,0x025D,0x025E,0x025F,0x0193,0x0261,0x0262,0x0194,0x0264,0x0265,0x0266,0x0267,0x0197,0x0196,0x026A,0x2C62,0x026C,0x026D,0x026E,0x019C,0x0270,0x0271,0x019D,0x0273,0x0274,0x019F,0x0276,0x0277,0x0278,0x0279,0x027A,0x027B,0x027C,0x2C64,0x027E,0x027F,0x01A6,0x0281,0x0282,0x01A9,0x0284,0x0285,0x0286,0x0287,0x01AE,0x0244,0x01B1,0x01B2,0x0245,0x028D,0x028E,0x028F,0x0290,0x0291,0x01B7,
		/* Greek, Coptic */
		0x037B,0x0003,0x03FD,0x03FE,0x03FF,  0x03AC,0x0004,0x0386,0x0388,0x0389,0x038A,  0x03B1,0x0311,
		0x03C2,0x0002,0x03A3,0x03A3,  0x03C4,0x0308,  0x03CC,0x0003,0x038C,0x038E,0x038F,  0x03D8,0x0118,
		0x03F2,0x000A,0x03F9,0x03F3,0x03F4,0x03F5,0x03F6,0x03F7,0x03F7,0x03F9,0x03FA,0x03FA,
		/* Cyrillic */
		0x0430,0x0320,  0x0450,0x0710,  0x0460,0x0122,  0x048A,0x0136,  0x04C1,0x010E,  0x04CF,0x0001,0x04C0,  0x04D0,0x0144,
		/* Armenian */
		0x0561,0x0426,

		0x0000
	};
	static const WORD cvt2[] = {	/* U+1000 - U+FFFF */
		/* Phonetic Extensions */
		0x1D7D,0x0001,0x2C63,
		/* Latin Extended Additional */
		0x1E00,0x0196,  0x1EA0,0x015A,
		/* Greek Extended */
		0x1F00,0x0608,  0x1F10,0x0606,  0x1F20,0x0608,  0x1F30,0x0608,  0x1F40,0x0606,
		0x1F51,0x0007,0x1F59,0x1F52,0x1F5B,0x1F54,0x1F5D,0x1F56,0x1F5F,  0x1F60,0x0608,
		0x1F70,0x000E,0x1FBA,0x1FBB,0x1FC8,0x1FC9,0x1FCA,0x1FCB,0x1FDA,0x1FDB,0x1FF8,0x1FF9,0x1FEA,0x1FEB,0x1FFA,0x1FFB,
		0x1F80,0x0608,  0x1F90,0x0608,  0x1FA0,0x0608,  0x1FB0,0x0004,0x1FB8,0x1FB9,0x1FB2,0x1FBC,
		0x1FCC,0x0001,0x1FC3,  0x1FD0,0x0602,  0x1FE0,0x0602,  0x1FE5,0x0001,0x1FEC,  0x1FF3,0x0001,0x1FFC,
		/* Letterlike Symbols */
		0x214E,0x0001,0x2132,
		/* Number forms */
		0x2170,0x0210,  0x2184,0x0001,0x2183,
		/* Enclosed Alphanumerics */
		0x24D0,0x051A,  0x2C30,0x042F,
		/* Latin Extended-C */
		0x2C60,0x0102,  0x2C67,0x0106, 0x2C75,0x0102,
		/* Coptic */
		0x2C80,0x0164,
		/* Georgian Supplement */
		0x2D00,0x0826,
		/* Full-width */
		0xFF41,0x031A,

		0x0000
	};
	const WORD *p;
	WORD uc, bc, nc, cmd;

	if (uni < 0x10000) {	/* Is it in BMP? */
		uc = (WORD)uni;
		p = uc < 0x1000 ? cvt1 : cvt2;
		for (;;) {
			bc = *p++;								/* Get block base */
			if (!bc || uc < bc) break;
			nc = *p++; cmd = nc >> 8; nc &= 0xFF;	/* Get processing command and block size */
			if (uc < bc + nc) {	/* In the block? */
				switch (cmd) {
				case 0:	uc = p[uc - bc]; break;		/* Table conversion */
				case 1:	uc -= (uc - bc) & 1; break;	/* Case pairs */
				case 2: uc -= 16; break;			/* Shift -16 */
				case 3:	uc -= 32; break;			/* Shift -32 */
				case 4:	uc -= 48; break;			/* Shift -48 */
				case 5:	uc -= 26; break;			/* Shift -26 */
				case 6:	uc += 8; break;				/* Shift +8 */
				case 7: uc -= 80; break;			/* Shift -80 */
				case 8:	uc -= 0x1C60; break;		/* Shift -0x1C60 */
				}
				break;
			}
			if (!cmd) p += nc;
		}
		uni = uc;
	}

	return uni;
}


/* Character functions */

int FatFs::dbc_1st(BYTE c) {
    //Always 0 for codepage 437
    return 0;
}

int FatFs::dbc_2nd(BYTE c) {
    //Always 0 for codepage 437
    return 0;
}

DWORD FatFs::tchar2uni(const TCHAR** str) {
    DWORD uc;
	const TCHAR *p = *str;

    BYTE b;
	WCHAR wc;

	wc = (BYTE)*p++;			/* Get a byte */
	if (dbc_1st((BYTE)wc)) {	/* Is it a DBC 1st byte? */
		b = (BYTE)*p++;			/* Get 2nd byte */
		if (!dbc_2nd(b)) return 0xFFFFFFFF;	/* Invalid code? */
		wc = (wc << 8) + b;		/* Make a DBC */
	}
	if (wc != 0) {
		wc = ff_oem2uni(wc, FF_CODE_PAGE);	/* ANSI/OEM ==> Unicode */
		if (wc == 0) return 0xFFFFFFFF;	/* Invalid code? */
	}
	uc = wc;

    *str = p;	/* Next read pointer */
	return uc;
}

BYTE FatFs::put_utf(DWORD chr, TCHAR *buf, UINT szb) {
    WCHAR wc;

	wc = ff_uni2oem(chr, FF_CODE_PAGE);
	if (wc >= 0x100) {	/* Is this a DBC? */
		if (szb < 2) return 0;
		*buf++ = (char)(wc >> 8);	/* Store DBC 1st byte */
		*buf++ = (TCHAR)wc;			/* Store DBC 2nd byte */
		return 2;
	}
	if (wc == 0 || szb < 1) return 0;	/* Invalid char or buffer overflow? */
	*buf++ = (TCHAR)wc;					/* Store the character */
	return 1;
}


/* Functions to load/store multi-byte words */

WORD FatFs::ld_word(const BYTE *ptr) {
    WORD rv;

	rv = ptr[1];
	rv = rv << 8 | ptr[0];
	return rv;
}

DWORD FatFs::ld_dword(const BYTE *ptr) {
    DWORD rv;

	rv = ptr[3];
	rv = rv << 8 | ptr[2];
	rv = rv << 8 | ptr[1];
	rv = rv << 8 | ptr[0];
	return rv;
}

void FatFs::st_word(BYTE *ptr, WORD val) {
    *ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val;
}

void FatFs::st_dword(BYTE *ptr, DWORD val) {
    *ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val; val >>= 8;
	*ptr++ = (BYTE)val;
}


/* Basic FAT-functions */

FRESULT FatFs::sync_window(FATFS *fs) {
    FRESULT res = FR_OK;

	if (fs->wflag) {	/* Is the disk access window dirty */
		if (fs->disk->write(fs->win, fs->winsect, 1)) {	/* Write back the window */
			fs->wflag = 0;	/* Clear window dirty flag */
			if (fs->winsect - fs->fatbase < fs->fsize) {	/* Is it in the 1st FAT? */
				if (fs->n_fats == 2) fs->disk->write(fs->win, fs->winsect + fs->fsize, 1);	/* Reflect it to 2nd FAT if needed */
			}
		} else {
			res = FR_DISK_ERR;
		}
	}
	return res;
}

FRESULT FatFs::move_window(FATFS *fs, DWORD sector) {
    FRESULT res = FR_OK;

	if (sector != fs->winsect) {	/* Window offset changed? */
		res = sync_window(fs);		/* Write-back changes */

		if (res == FR_OK) {			/* Fill sector window with new data */
			if (!fs->disk->read(fs->win, sector, 1)) {
				sector = 0xFFFFFFFF;	/* Invalidate window if read data is not valid */
				res = FR_DISK_ERR;
			}
			fs->winsect = sector;
		}
	}
	return res;
}

FRESULT FatFs::sync_fs(FATFS *fs) {
    FRESULT res;

	res = sync_window(fs);
	if (res == FR_OK) {
		if (fs->fs_type == FS_FAT32 && fs->fsi_flag == 1) {	/* FAT32: Update FSInfo sector if needed */
			/* Create FSInfo structure */
			memset(fs->win, 0, SS(fs));
			st_word(fs->win + BS_55AA, 0xAA55);
			st_dword(fs->win + FSI_LeadSig, 0x41615252);
			st_dword(fs->win + FSI_StrucSig, 0x61417272);
			st_dword(fs->win + FSI_Free_Count, fs->free_clst);
			st_dword(fs->win + FSI_Nxt_Free, fs->last_clst);
			/* Write it into the FSInfo sector */
			fs->winsect = fs->volbase + 1;
			fs->disk->write(fs->win, fs->winsect, 1);
			fs->fsi_flag = 0;
		}
	}

	return res;
}

DWORD FatFs::clst2sect(FATFS *fs, DWORD clst) {
    clst -= 2;		/* Cluster number is origin from 2 */
	if (clst >= fs->n_fatent - 2) return 0;		/* Is it invalid cluster number? */
	return fs->database + fs->csize * clst;		/* Start sector number of the cluster */
}

DWORD FatFs::get_fat(FFOBJID *obj, DWORD clst) {
    UINT wc, bc;
	DWORD val;
	FATFS *fs = obj->fs;

	if (clst < 2 || clst >= fs->n_fatent) {	/* Check if in valid range */
		val = 1;	/* Internal error */

	} else {
		val = 0xFFFFFFFF;	/* Default value falls on disk error */

		switch (fs->fs_type) {
		case FS_FAT12 :
			bc = (UINT)clst; bc += bc / 2;
			if (move_window(fs, fs->fatbase + (bc / SS(fs))) != FR_OK) break;
			wc = fs->win[bc++ % SS(fs)];		/* Get 1st byte of the entry */
			if (move_window(fs, fs->fatbase + (bc / SS(fs))) != FR_OK) break;
			wc |= fs->win[bc % SS(fs)] << 8;	/* Merge 2nd byte of the entry */
			val = (clst & 1) ? (wc >> 4) : (wc & 0xFFF);	/* Adjust bit position */
			break;

		case FS_FAT16 :
			if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 2))) != FR_OK) break;
			val = ld_word(fs->win + clst * 2 % SS(fs));		/* Simple WORD array */
			break;

		case FS_FAT32 :
			if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 4))) != FR_OK) break;
			val = ld_dword(fs->win + clst * 4 % SS(fs)) & 0x0FFFFFFF;	/* Simple DWORD array but mask out upper 4 bits */
			break;
		default:
			val = 1;	/* Internal error */
		}
	}

	return val;
}

FRESULT FatFs::put_fat(FATFS *fs, DWORD clst, DWORD val) {
    UINT bc;
	BYTE *p;
	FRESULT res = FR_INT_ERR;

	if (clst >= 2 && clst < fs->n_fatent) {	/* Check if in valid range */
		switch (fs->fs_type) {
		case FS_FAT12 :
			bc = (UINT)clst; bc += bc / 2;	/* bc: byte offset of the entry */
			res = move_window(fs, fs->fatbase + (bc / SS(fs)));
			if (res != FR_OK) break;
			p = fs->win + bc++ % SS(fs);
			*p = (clst & 1) ? ((*p & 0x0F) | ((BYTE)val << 4)) : (BYTE)val;		/* Put 1st byte */
			fs->wflag = 1;
			res = move_window(fs, fs->fatbase + (bc / SS(fs)));
			if (res != FR_OK) break;
			p = fs->win + bc % SS(fs);
			*p = (clst & 1) ? (BYTE)(val >> 4) : ((*p & 0xF0) | ((BYTE)(val >> 8) & 0x0F));	/* Put 2nd byte */
			fs->wflag = 1;
			break;

		case FS_FAT16 :
			res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 2)));
			if (res != FR_OK) break;
			st_word(fs->win + clst * 2 % SS(fs), (WORD)val);	/* Simple WORD array */
			fs->wflag = 1;
			break;

		case FS_FAT32 :
			res = move_window(fs, fs->fatbase + (clst / (SS(fs) / 4)));
			if (res != FR_OK) break;
			if (fs->fs_type != FS_EXFAT) {
				val = (val & 0x0FFFFFFF) | (ld_dword(fs->win + clst * 4 % SS(fs)) & 0xF0000000);
			}
			st_dword(fs->win + clst * 4 % SS(fs), val);
			fs->wflag = 1;
			break;
		}
	}

	return res;
}

FRESULT FatFs::remove_chain(FFOBJID *obj, DWORD clst, DWORD pclst) {
    FRESULT res = FR_OK;
	DWORD nxt;
	FATFS *fs = obj->fs;

	if (clst < 2 || clst >= fs->n_fatent) return FR_INT_ERR;	/* Check if in valid range */

	/* Mark the previous cluster 'EOC' on the FAT if it exists */
	if (pclst != 0 && (fs->fs_type != FS_EXFAT || obj->stat != 2)) {
		res = put_fat(fs, pclst, 0xFFFFFFFF);
		if (res != FR_OK) return res;
	}

	/* Remove the chain */
	do {
		nxt = get_fat(obj, clst);			/* Get cluster status */
		if (nxt == 0) break;				/* Empty cluster? */
		if (nxt == 1) return FR_INT_ERR;	/* Internal error? */
		if (nxt == 0xFFFFFFFF) return FR_DISK_ERR;	/* Disk error? */
		if (fs->fs_type != FS_EXFAT) {
			res = put_fat(fs, clst, 0);		/* Mark the cluster 'free' on the FATFS */
			if (res != FR_OK) return res;
		}
		if (fs->free_clst < fs->n_fatent - 2) {	/* Update FSINFO */
			fs->free_clst++;
			fs->fsi_flag |= 1;
		}

		clst = nxt;					/* Next cluster */
	} while (clst < fs->n_fatent);	/* Repeat while not the last link */

	return FR_OK;
}

DWORD FatFs::create_chain(FFOBJID *obj, DWORD clst) {
    DWORD cs, ncl, scl;
	FRESULT res;
	FATFS *fs = obj->fs;

	if (clst == 0) {	/* Create a new chain */
		scl = fs->last_clst;				/* Suggested cluster to start to find */
		if (scl == 0 || scl >= fs->n_fatent) scl = 1;
	}
	else {				/* Stretch a chain */
		cs = get_fat(obj, clst);			/* Check the cluster status */
		if (cs < 2) return 1;				/* Test for insanity */
		if (cs == 0xFFFFFFFF) return cs;	/* Test for disk error */
		if (cs < fs->n_fatent) return cs;	/* It is already followed by next cluster */
		scl = clst;							/* Cluster to start to find */
	}
	if (fs->free_clst == 0) return 0;		/* No free cluster */

	{	/* On the FAT/FAT32 volume */
		ncl = 0;
		if (scl == clst) {						/* Stretching an existing chain? */
			ncl = scl + 1;						/* Test if next cluster is free */
			if (ncl >= fs->n_fatent) ncl = 2;
			cs = get_fat(obj, ncl);				/* Get next cluster status */
			if (cs == 1 || cs == 0xFFFFFFFF) return cs;	/* Test for error */
			if (cs != 0) {						/* Not free? */
				cs = fs->last_clst;				/* Start at suggested cluster if it is valid */
				if (cs >= 2 && cs < fs->n_fatent) scl = cs;
				ncl = 0;
			}
		}
		if (ncl == 0) {	/* The new cluster cannot be contiguous and find another fragment */
			ncl = scl;	/* Start cluster */
			for (;;) {
				ncl++;							/* Next cluster */
				if (ncl >= fs->n_fatent) {		/* Check wrap-around */
					ncl = 2;
					if (ncl > scl) return 0;	/* No free cluster found? */
				}
				cs = get_fat(obj, ncl);			/* Get the cluster status */
				if (cs == 0) break;				/* Found a free cluster? */
				if (cs == 1 || cs == 0xFFFFFFFF) return cs;	/* Test for error */
				if (ncl == scl) return 0;		/* No free cluster found? */
			}
		}
		res = put_fat(fs, ncl, 0xFFFFFFFF);		/* Mark the new cluster 'EOC' */
		if (res == FR_OK && clst != 0) {
			res = put_fat(fs, clst, ncl);		/* Link it from the previous one if needed */
		}
	}

	if (res == FR_OK) {			/* Update FSINFO if function succeeded. */
		fs->last_clst = ncl;
		if (fs->free_clst <= fs->n_fatent - 2) fs->free_clst--;
		fs->fsi_flag |= 1;
	} else {
		ncl = (res == FR_DISK_ERR) ? 0xFFFFFFFF : 1;	/* Failed. Generate error status */
	}

	return ncl;		/* Return new cluster number or error status */
}

FRESULT FatFs::dir_clear(FATFS *fs, DWORD clst) {
    DWORD sect;
	UINT n, szb;
	BYTE *ibuf;

	if (sync_window(fs) != FR_OK) return FR_DISK_ERR;	/* Flush disk access window */
	sect = clst2sect(fs, clst);		/* Top of the cluster */
	fs->winsect = sect;				/* Set window to top of the cluster */
	memset(fs->win, 0, SS(fs));	/* Clear window buffer */

    /* Quick table clear by using multi-secter write */
	/* Allocate a temporary buffer */
	for (szb = ((DWORD)fs->csize * SS(fs) >= MAX_MALLOC) ? MAX_MALLOC : fs->csize * SS(fs); szb > SS(fs) && !(ibuf = (BYTE *) malloc(szb)); szb /= 2) ;
	if (szb > SS(fs)) {		/* Buffer allocated? */
		memset(ibuf, 0, szb);
		szb /= SS(fs);		/* Bytes -> Sectors */
		for (n = 0; n < fs->csize && fs->disk->write(ibuf, sect + n, szb); n += szb) ;	/* Fill the cluster with 0 */
		free(ibuf);
	} else
	{
		ibuf = fs->win; szb = 1;	/* Use window buffer (single-sector writes may take a time) */
		for (n = 0; n < fs->csize && fs->disk->write(ibuf, sect + n, szb); n += szb) ;	/* Fill the cluster with 0 */
	}

	return (n == fs->csize) ? FR_OK : FR_DISK_ERR;
}

FRESULT FatFs::dir_sdi(DIR *dp, DWORD ofs) {
    DWORD csz, clst;
	FATFS *fs = dp->obj.fs;

	if (ofs >= (DWORD)((fs->fs_type == FS_EXFAT) ? MAX_DIR_EX : MAX_DIR) || ofs % SZDIRE) {	/* Check range of offset and alignment */
		return FR_INT_ERR;
	}
	dp->dptr = ofs;				/* Set current offset */
	clst = dp->obj.sclust;		/* Table start cluster (0:root) */
	if (clst == 0 && fs->fs_type >= FS_FAT32) {	/* Replace cluster# 0 with root cluster# */
		clst = fs->dirbase;
	}

	if (clst == 0) {	/* Static table (root-directory on the FAT volume) */
		if (ofs / SZDIRE >= fs->n_rootdir) return FR_INT_ERR;	/* Is index out of range? */
		dp->sect = fs->dirbase;

	} else {			/* Dynamic table (sub-directory or root-directory on the FAT32/exFAT volume) */
		csz = (DWORD)fs->csize * SS(fs);	/* Bytes per cluster */
		while (ofs >= csz) {				/* Follow cluster chain */
			clst = get_fat(&dp->obj, clst);				/* Get next cluster */
			if (clst == 0xFFFFFFFF) return FR_DISK_ERR;	/* Disk error */
			if (clst < 2 || clst >= fs->n_fatent) return FR_INT_ERR;	/* Reached to end of table or internal error */
			ofs -= csz;
		}
		dp->sect = clst2sect(fs, clst);
	}
	dp->clust = clst;					/* Current cluster# */
	if (dp->sect == 0) return FR_INT_ERR;
	dp->sect += ofs / SS(fs);			/* Sector# of the directory entry */
	dp->dir = fs->win + (ofs % SS(fs));	/* Pointer to the entry in the win[] */

	return FR_OK;
}

FRESULT FatFs::dir_next(DIR *dp, int stretch) {
    DWORD ofs, clst;
	FATFS *fs = dp->obj.fs;

	ofs = dp->dptr + SZDIRE;	/* Next entry */
	if (dp->sect == 0 || ofs >= (DWORD)((fs->fs_type == FS_EXFAT) ? MAX_DIR_EX : MAX_DIR)) return FR_NO_FILE;	/* Report EOT when offset has reached max value */

	if (ofs % SS(fs) == 0) {	/* Sector changed? */
		dp->sect++;				/* Next sector */

		if (dp->clust == 0) {	/* Static table */
			if (ofs / SZDIRE >= fs->n_rootdir) {	/* Report EOT if it reached end of static table */
				dp->sect = 0; return FR_NO_FILE;
			}
		}
		else {					/* Dynamic table */
			if ((ofs / SS(fs) & (fs->csize - 1)) == 0) {	/* Cluster changed? */
				clst = get_fat(&dp->obj, dp->clust);		/* Get next cluster */
				if (clst <= 1) return FR_INT_ERR;			/* Internal error */
				if (clst == 0xFFFFFFFF) return FR_DISK_ERR;	/* Disk error */
				if (clst >= fs->n_fatent) {					/* It reached end of dynamic table */
					if (!stretch) {								/* If no stretch, report EOT */
						dp->sect = 0; return FR_NO_FILE;
					}
					clst = create_chain(&dp->obj, dp->clust);	/* Allocate a cluster */
					if (clst == 0) return FR_DENIED;			/* No free cluster */
					if (clst == 1) return FR_INT_ERR;			/* Internal error */
					if (clst == 0xFFFFFFFF) return FR_DISK_ERR;	/* Disk error */
					if (dir_clear(fs, clst) != FR_OK) return FR_DISK_ERR;	/* Clean up the stretched table */
				}
				dp->clust = clst;		/* Initialize data for new cluster */
				dp->sect = clst2sect(fs, clst);
			}
		}
	}
	dp->dptr = ofs;						/* Current entry */
	dp->dir = fs->win + ofs % SS(fs);	/* Pointer to the entry in the win[] */

	return FR_OK;
}

FRESULT FatFs::dir_alloc(DIR *dp, UINT nent) {
    FRESULT res;
	UINT n;
	FATFS *fs = dp->obj.fs;

	res = dir_sdi(dp, 0);
	if (res == FR_OK) {
		n = 0;
		do {
			res = move_window(fs, dp->sect);
			if (res != FR_OK) break;
			if (dp->dir[DIR_Name] == DDEM || dp->dir[DIR_Name] == 0) {
				if (++n == nent) break;	/* A block of contiguous free entries is found */
			} else {
				n = 0;					/* Not a blank entry. Restart to search */
			}
			res = dir_next(dp, 1);
		} while (res == FR_OK);	/* Next entry with table stretch enabled */
	}

	if (res == FR_NO_FILE) res = FR_DENIED;	/* No directory entry to allocate */
	return res;
}

DWORD FatFs::ld_clust(FATFS *fs, const BYTE *dir) {
    DWORD cl;

	cl = ld_word(dir + DIR_FstClusLO);
	if (fs->fs_type == FS_FAT32) {
		cl |= (DWORD)ld_word(dir + DIR_FstClusHI) << 16;
	}

	return cl;
}

void FatFs::st_clust(FATFS *fs, BYTE *dir, DWORD cl) {
    st_word(dir + DIR_FstClusLO, (WORD)cl);
	if (fs->fs_type == FS_FAT32) {
		st_word(dir + DIR_FstClusHI, (WORD)(cl >> 16));
	}
}

int FatFs::cmp_lfn(const WCHAR *lfnbuf, BYTE *dir) {
	UINT i, s;
	WCHAR wc, uc;

	if (ld_word(dir + LDIR_FstClusLO) != 0) return 0;	/* Check LDIR_FstClusLO */

	i = ((dir[LDIR_Ord] & 0x3F) - 1) * 13;	/* Offset in the LFN buffer */

	for (wc = 1, s = 0; s < 13; s++) {		/* Process all characters in the entry */
		uc = ld_word(dir + LfnOfs[s]);		/* Pick an LFN character */
		if (wc != 0) {
			if (i >= FF_MAX_LFN || ff_wtoupper(uc) != ff_wtoupper(lfnbuf[i++])) {	/* Compare it */
				return 0;					/* Not matched */
			}
			wc = uc;
		} else {
			if (uc != 0xFFFF) return 0;		/* Check filler */
		}
	}

	if ((dir[LDIR_Ord] & LLEF) && wc && lfnbuf[i]) return 0;	/* Last segment matched but different length */

	return 1;		/* The part of LFN matched */
}

int FatFs::pick_lfn(WCHAR *lfnbuf, BYTE *dir) {
	UINT i, s;
	WCHAR wc, uc;

	if (ld_word(dir + LDIR_FstClusLO) != 0) return 0;	/* Check LDIR_FstClusLO is 0 */

	i = ((dir[LDIR_Ord] & ~LLEF) - 1) * 13;	/* Offset in the LFN buffer */

	for (wc = 1, s = 0; s < 13; s++) {		/* Process all characters in the entry */
		uc = ld_word(dir + LfnOfs[s]);		/* Pick an LFN character */
		if (wc != 0) {
			if (i >= FF_MAX_LFN) return 0;	/* Buffer overflow? */
			lfnbuf[i++] = wc = uc;			/* Store it */
		} else {
			if (uc != 0xFFFF) return 0;		/* Check filler */
		}
	}

	if (dir[LDIR_Ord] & LLEF) {				/* Put terminator if it is the last LFN part */
		if (i >= FF_MAX_LFN) return 0;		/* Buffer overflow? */
		lfnbuf[i] = 0;
	}

	return 1;		/* The part of LFN is valid */
}

void FatFs::put_lfn(const WCHAR *lfn, BYTE *dir, BYTE ord, BYTE sum) {
	UINT i, s;
	WCHAR wc;

	dir[LDIR_Chksum] = sum;			/* Set checksum */
	dir[LDIR_Attr] = AM_LFN;		/* Set attribute. LFN entry */
	dir[LDIR_Type] = 0;
	st_word(dir + LDIR_FstClusLO, 0);

	i = (ord - 1) * 13;				/* Get offset in the LFN working buffer */
	s = wc = 0;
	do {
		if (wc != 0xFFFF) wc = lfn[i++];	/* Get an effective character */
		st_word(dir + LfnOfs[s], wc);		/* Put it */
		if (wc == 0) wc = 0xFFFF;		/* Padding characters for left locations */
	} while (++s < 13);
	if (wc == 0xFFFF || !lfn[i]) ord |= LLEF;	/* Last LFN part is the start of LFN sequence */
	dir[LDIR_Ord] = ord;			/* Set the LFN order */
}

void FatFs::gen_numname(BYTE *dst, const BYTE *src, const WCHAR *lfn, UINT seq) {
	BYTE ns[8], c;
	UINT i, j;
	WCHAR wc;
	DWORD sr;

	memcpy(dst, src, 11);

	if (seq > 5) {	/* In case of many collisions, generate a hash number instead of sequential number */
		sr = seq;
		while (*lfn) {	/* Create a CRC */
			wc = *lfn++;
			for (i = 0; i < 16; i++) {
				sr = (sr << 1) + (wc & 1);
				wc >>= 1;
				if (sr & 0x10000) sr ^= 0x11021;
			}
		}
		seq = (UINT)sr;
	}

	/* itoa (hexdecimal) */
	i = 7;
	do {
		c = (BYTE)((seq % 16) + '0');
		if (c > '9') c += 7;
		ns[i--] = c;
		seq /= 16;
	} while (seq);
	ns[i] = '~';

	/* Append the number to the SFN body */
	for (j = 0; j < i && dst[j] != ' '; j++) {
		if (dbc_1st(dst[j])) {
			if (j == i - 1) break;
			j++;
		}
	}
	do {
		dst[j++] = (i < 8) ? ns[i++] : ' ';
	} while (j < 8);
}

BYTE FatFs::sum_sfn(const BYTE *dir) {
	BYTE sum = 0;
	UINT n = 11;

	do {
		sum = (sum >> 1) + (sum << 7) + *dir++;
	} while (--n);

	return sum;
}

FRESULT FatFs::dir_read(DIR *dp, int vol) {
	FRESULT res = FR_NO_FILE;
	FATFS *fs = dp->obj.fs;
	BYTE a, c;
	BYTE ord = 0xFF, sum = 0xFF;

	while (dp->sect) {
		res = move_window(fs, dp->sect);
		if (res != FR_OK) break;
		c = dp->dir[DIR_Name];	/* Test for the entry type */
		if (c == 0) {
			res = FR_NO_FILE; break; /* Reached to end of the directory */
		}

		{	/* On the FAT/FAT32 volume */
			dp->obj.attr = a = dp->dir[DIR_Attr] & AM_MASK;	/* Get attribute */
			if (c == DDEM || c == '.' || (int)((a & ~AM_ARC) == AM_VOL) != vol) {	/* An entry without valid data */
				ord = 0xFF;
			} else {
				if (a == AM_LFN) {			/* An LFN entry is found */
					if (c & LLEF) {			/* Is it start of an LFN sequence? */
						sum = dp->dir[LDIR_Chksum];
						c &= (BYTE)~LLEF; ord = c;
						dp->blk_ofs = dp->dptr;
					}
					/* Check LFN validity and capture it */
					ord = (c == ord && sum == dp->dir[LDIR_Chksum] && pick_lfn(fs->lfnbuf, dp->dir)) ? ord - 1 : 0xFF;
				} else {					/* An SFN entry is found */
					if (ord != 0 || sum != sum_sfn(dp->dir)) {	/* Is there a valid LFN? */
						dp->blk_ofs = 0xFFFFFFFF;			/* It has no LFN. */
					}
					break;
				}
			}
			if (c != DDEM && c != '.' && a != AM_LFN && (int)((a & ~AM_ARC) == AM_VOL) == vol) {	/* Is it a valid entry? */
				break;
			}
		}
		res = dir_next(dp, 0);		/* Next entry */
		if (res != FR_OK) break;
	}

	if (res != FR_OK) dp->sect = 0;		/* Terminate the read operation on error or EOT */
	
	return res;
}

FRESULT FatFs::dir_find(DIR *dp) {
	FRESULT res;
	FATFS *fs = dp->obj.fs;
	BYTE c;
	BYTE a, ord, sum;

	res = dir_sdi(dp, 0);			/* Rewind directory object */
	if (res != FR_OK) return res;

	/* On the FAT/FAT32 volume */
	ord = sum = 0xFF; dp->blk_ofs = 0xFFFFFFFF;	/* Reset LFN sequence */
	do {
		res = move_window(fs, dp->sect);
		if (res != FR_OK) break;
		c = dp->dir[DIR_Name];
		if (c == 0) { res = FR_NO_FILE; break; }	/* Reached to end of table */
		/* LFN configuration */
		dp->obj.attr = a = dp->dir[DIR_Attr] & AM_MASK;
		if (c == DDEM || ((a & AM_VOL) && a != AM_LFN)) {	/* An entry without valid data */
			ord = 0xFF; dp->blk_ofs = 0xFFFFFFFF;	/* Reset LFN sequence */
		} else {
			if (a == AM_LFN) {			/* An LFN entry is found */
				if (!(dp->fn[NSFLAG] & NS_NOLFN)) {
					if (c & LLEF) {		/* Is it start of LFN sequence? */
						sum = dp->dir[LDIR_Chksum];
						c &= (BYTE)~LLEF; ord = c;	/* LFN start order */
						dp->blk_ofs = dp->dptr;	/* Start offset of LFN */
					}
					/* Check validity of the LFN entry and compare it with given name */
					ord = (c == ord && sum == dp->dir[LDIR_Chksum] && cmp_lfn(fs->lfnbuf, dp->dir)) ? ord - 1 : 0xFF;
				}
			} else {					/* An SFN entry is found */
				if (ord == 0 && sum == sum_sfn(dp->dir)) break;	/* LFN matched? */
				if (!(dp->fn[NSFLAG] & NS_LOSS) && !memcmp(dp->dir, dp->fn, 11)) break;	/* SFN matched? */
				ord = 0xFF; dp->blk_ofs = 0xFFFFFFFF;	/* Reset LFN sequence */
			}
		}

		res = dir_next(dp, 0);	/* Next entry */
	} while (res == FR_OK);

	return res;
}

FRESULT FatFs::dir_register(DIR *dp) {
	FRESULT res;
	FATFS *fs = dp->obj.fs;
	UINT n, nlen, nent;
	BYTE sn[12], sum;


	if (dp->fn[NSFLAG] & (NS_DOT | NS_NONAME)) return FR_INVALID_NAME;	/* Check name validity */
	for (nlen = 0; fs->lfnbuf[nlen]; nlen++) ;	/* Get lfn length */

	/* On the FAT/FAT32 volume */
	memcpy(sn, dp->fn, 12);
	if (sn[NSFLAG] & NS_LOSS) {			/* When LFN is out of 8.3 format, generate a numbered name */
		dp->fn[NSFLAG] = NS_NOLFN;		/* Find only SFN */
		for (n = 1; n < 100; n++) {
			gen_numname(dp->fn, sn, fs->lfnbuf, n);	/* Generate a numbered name */
			res = dir_find(dp);				/* Check if the name collides with existing SFN */
			if (res != FR_OK) break;
		}
		if (n == 100) return FR_DENIED;		/* Abort if too many collisions */
		if (res != FR_NO_FILE) return res;	/* Abort if the result is other than 'not collided' */
		dp->fn[NSFLAG] = sn[NSFLAG];
	}

	/* Create an SFN with/without LFNs. */
	nent = (sn[NSFLAG] & NS_LFN) ? (nlen + 12) / 13 + 1 : 1;	/* Number of entries to allocate */
	res = dir_alloc(dp, nent);		/* Allocate entries */
	if (res == FR_OK && --nent) {	/* Set LFN entry if needed */
		res = dir_sdi(dp, dp->dptr - nent * SZDIRE);
		if (res == FR_OK) {
			sum = sum_sfn(dp->fn);	/* Checksum value of the SFN tied to the LFN */
			do {					/* Store LFN entries in bottom first */
				res = move_window(fs, dp->sect);
				if (res != FR_OK) break;
				put_lfn(fs->lfnbuf, dp->dir, (BYTE)nent, sum);
				fs->wflag = 1;
				res = dir_next(dp, 0);	/* Next entry */
			} while (res == FR_OK && --nent);
		}
	}

	/* Set SFN entry */
	if (res == FR_OK) {
		res = move_window(fs, dp->sect);
		if (res == FR_OK) {
			memset(dp->dir, 0, SZDIRE);	/* Clean the entry */
			memcpy(dp->dir + DIR_Name, dp->fn, 11);	/* Put SFN */
			dp->dir[DIR_NTres] = dp->fn[NSFLAG] & (NS_BODY | NS_EXT);	/* Put NT flag */
			fs->wflag = 1;
		}
	}

	return res;
}

FRESULT FatFs::dir_remove(DIR *dp) {
	FRESULT res;
	FATFS *fs = dp->obj.fs;
	DWORD last = dp->dptr;

	res = (dp->blk_ofs == 0xFFFFFFFF) ? FR_OK : dir_sdi(dp, dp->blk_ofs);	/* Goto top of the entry block if LFN is exist */
	if (res == FR_OK) {
		do {
			res = move_window(fs, dp->sect);
			if (res != FR_OK) break;
			
			dp->dir[DIR_Name] = DDEM;	/* Mark the entry 'deleted'. */
			fs->wflag = 1;
			if (dp->dptr >= last) break;	/* If reached last entry then all entries of the object has been deleted. */
			res = dir_next(dp, 0);	/* Next entry */
		} while (res == FR_OK);
		if (res == FR_NO_FILE) res = FR_INT_ERR;
	}

	return res;
}

void FatFs::get_fileinfo(DIR *dp, FILINFO *fno) {
	UINT si, di;
	WCHAR wc, hs;
	FATFS *fs = dp->obj.fs;


	fno->fname[0] = 0;			/* Invaidate file info */
	if (dp->sect == 0) return;	/* Exit if read pointer has reached end of directory */

	{	/* On the FAT/FAT32 volume */
		if (dp->blk_ofs != 0xFFFFFFFF) {	/* Get LFN if available */
			si = di = hs = 0;
			while (fs->lfnbuf[si] != 0) {
				wc = fs->lfnbuf[si++];		/* Get an LFN character (UTF-16) */
				if (hs == 0 && IsSurrogate(wc)) {	/* Is it a surrogate? */
					hs = wc; continue;		/* Get low surrogate */
				}
				wc = put_utf((DWORD)hs << 16 | wc, &fno->fname[di], FF_LFN_BUF - di);	/* Store it in UTF-16 or UTF-8 encoding */
				if (wc == 0) { di = 0; break; }	/* Invalid char or buffer overflow? */
				di += wc;
				hs = 0;
			}
			if (hs != 0) di = 0;	/* Broken surrogate pair? */
			fno->fname[di] = 0;		/* Terminate the LFN (null string means LFN is invalid) */
		}
	}

	si = di = 0;
	while (si < 11) {		/* Get SFN from SFN entry */
		wc = dp->dir[si++];			/* Get a char */
		if (wc == ' ') continue;	/* Skip padding spaces */
		if (wc == RDDEM) wc = DDEM;	/* Restore replaced DDEM character */
		if (si == 9 && di < FF_SFN_BUF) fno->altname[di++] = '.';	/* Insert a . if extension is exist */

		/* ANSI/OEM output */
		fno->altname[di++] = (TCHAR)wc;	/* Store it without any conversion */
	}
	fno->altname[di] = 0;	/* Terminate the SFN  (null string means SFN is invalid) */

	if (fno->fname[0] == 0) {	/* If LFN is invalid, altname[] needs to be copied to fname[] */
		if (di == 0) {	/* If LFN and SFN both are invalid, this object is inaccesible */
			fno->fname[di++] = '?';
		} else {
			for (si = di = 0; fno->altname[si]; si++, di++) {	/* Copy altname[] to fname[] with case information */
				wc = (WCHAR)fno->altname[si];
				if (IsUpper(wc) && (dp->dir[DIR_NTres] & ((si >= 9) ? NS_EXT : NS_BODY))) wc += 0x20;
				fno->fname[di] = (TCHAR)wc;
			}
		}
		fno->fname[di] = 0;	/* Terminate the LFN */
		if (!dp->dir[DIR_NTres]) fno->altname[0] = 0;	/* Altname is not needed if neither LFN nor case info is exist. */
	}

	fno->fattrib = dp->dir[DIR_Attr];					/* Attribute */
	fno->fsize = ld_dword(dp->dir + DIR_FileSize);		/* Size */
	fno->ftime = ld_word(dp->dir + DIR_ModTime + 0);	/* Time */
	fno->fdate = ld_word(dp->dir + DIR_ModTime + 2);	/* Date */
}

FRESULT FatFs::create_name(DIR *dp, const TCHAR **path) {
	BYTE b, cf;
	WCHAR wc, *lfn;
	DWORD uc;
	UINT i, ni, si, di;
	const TCHAR *p;


	/* Create LFN into LFN working buffer */
	p = *path; lfn = dp->obj.fs->lfnbuf; di = 0;
	for (;;) {
		uc = tchar2uni(&p);			/* Get a character */
		if (uc == 0xFFFFFFFF) return FR_INVALID_NAME;		/* Invalid code or UTF decode error */
		if (uc >= 0x10000) lfn[di++] = (WCHAR)(uc >> 16);	/* Store high surrogate if needed */
		wc = (WCHAR)uc;
		if (wc < ' ' || wc == '/' || wc == '\\') break;	/* Break if end of the path or a separator is found */
		if (wc < 0x80 && strchr("\"*:<>\?|\x7F", wc)) return FR_INVALID_NAME;	/* Reject illegal characters for LFN */
		if (di >= FF_MAX_LFN) return FR_INVALID_NAME;	/* Reject too long name */
		lfn[di++] = wc;					/* Store the Unicode character */
	}
	while (*p == '/' || *p == '\\') p++;	/* Skip duplicated separators if exist */
	*path = p;							/* Return pointer to the next segment */
	cf = (wc < ' ') ? NS_LAST : 0;		/* Set last segment flag if end of the path */

	while (di) {						/* Snip off trailing spaces and dots if exist */
		wc = lfn[di - 1];
		if (wc != ' ' && wc != '.') break;
		di--;
	}
	lfn[di] = 0;							/* LFN is created into the working buffer */
	if (di == 0) return FR_INVALID_NAME;	/* Reject null name */

	/* Create SFN in directory form */
	for (si = 0; lfn[si] == ' '; si++) ;	/* Remove leading spaces */
	if (si > 0 || lfn[si] == '.') cf |= NS_LOSS | NS_LFN;	/* Is there any leading space or dot? */
	while (di > 0 && lfn[di - 1] != '.') di--;	/* Find last dot (di<=si: no extension) */

	memset(dp->fn, ' ', 11);
	i = b = 0; ni = 8;
	for (;;) {
		wc = lfn[si++];					/* Get an LFN character */
		if (wc == 0) break;				/* Break on end of the LFN */
		if (wc == ' ' || (wc == '.' && si != di)) {	/* Remove embedded spaces and dots */
			cf |= NS_LOSS | NS_LFN;
			continue;
		}

		if (i >= ni || si == di) {		/* End of field? */
			if (ni == 11) {				/* Name extension overflow? */
				cf |= NS_LOSS | NS_LFN;
				break;
			}
			if (si != di) cf |= NS_LOSS | NS_LFN;	/* Name body overflow? */
			if (si > di) break;						/* No name extension? */
			si = di; i = 8; ni = 11; b <<= 2;		/* Enter name extension */
			continue;
		}

		if (wc >= 0x80) {	/* Is this a non-ASCII character? */
			cf |= NS_LFN;	/* LFN entry needs to be created */
			wc = ff_uni2oem(wc, FF_CODE_PAGE);			/* Unicode ==> ANSI/OEM code */
			if (wc & 0x80) wc = ExCvt[wc & 0x7F];	/* Convert extended character to upper (SBCS) */
		}

		if (wc >= 0x100) {				/* Is this a DBC? */
			if (i >= ni - 1) {			/* Field overflow? */
				cf |= NS_LOSS | NS_LFN;
				i = ni; continue;		/* Next field */
			}
			dp->fn[i++] = (BYTE)(wc >> 8);	/* Put 1st byte */
		} else {						/* SBC */
			if (wc == 0 || strchr("+,;=[]", wc)) {	/* Replace illegal characters for SFN if needed */
				wc = '_'; cf |= NS_LOSS | NS_LFN;/* Lossy conversion */
			} else {
				if (IsUpper(wc)) {		/* ASCII upper case? */
					b |= 2;
				}
				if (IsLower(wc)) {		/* ASCII lower case? */
					b |= 1; wc -= 0x20;
				}
			}
		}
		dp->fn[i++] = (BYTE)wc;
	}

	if (dp->fn[0] == DDEM) dp->fn[0] = RDDEM;	/* If the first character collides with DDEM, replace it with RDDEM */

	if (ni == 8) b <<= 2;				/* Shift capital flags if no extension */
	if ((b & 0x0C) == 0x0C || (b & 0x03) == 0x03) cf |= NS_LFN;	/* LFN entry needs to be created if composite capitals */
	if (!(cf & NS_LFN)) {				/* When LFN is in 8.3 format without extended character, NT flags are created */
		if (b & 0x01) cf |= NS_EXT;		/* NT flag (Extension has small capital letters only) */
		if (b & 0x04) cf |= NS_BODY;	/* NT flag (Body has small capital letters only) */
	}

	dp->fn[NSFLAG] = cf;	/* SFN is created into dp->fn[] */

	return FR_OK;
}

FRESULT FatFs::follow_path(DIR *dp, const TCHAR *path) {
	FRESULT res;
	BYTE ns;
	FATFS *fs = dp->obj.fs;

	{										/* With heading separator */
		while (*path == '/' || *path == '\\') path++;	/* Strip heading separator */
		dp->obj.sclust = 0;					/* Start from root directory */
	}

	if ((UINT)*path < ' ') {				/* Null path name is the origin directory itself */
		dp->fn[NSFLAG] = NS_NONAME;
		res = dir_sdi(dp, 0);

	} else {								/* Follow path */
		for (;;) {
			res = create_name(dp, &path);	/* Get a segment name of the path */
			if (res != FR_OK) break;
			res = dir_find(dp);				/* Find an object with the segment name */
			ns = dp->fn[NSFLAG];
			if (res != FR_OK) {				/* Failed to find the object */
				if (res == FR_NO_FILE) {	/* Object is not found */							/* Could not find the object */
					if (!(ns & NS_LAST)) res = FR_NO_PATH;	/* Adjust error code if not last segment */
				}
				break;
			}
			if (ns & NS_LAST) break;			/* Last segment matched. Function completed. */
			/* Get into the sub-directory */
			if (!(dp->obj.attr & AM_DIR)) {		/* It is not a sub-directory and cannot follow */
				res = FR_NO_PATH; break;
			}
			{
				dp->obj.sclust = ld_clust(fs, fs->win + dp->dptr % SS(fs));	/* Open next directory */
			}
		}
	}

	return res;
}

BYTE FatFs::check_fs(FATFS *fs, DWORD sect) {
	fs->wflag = 0; fs->winsect = 0xFFFFFFFF;		/* Invaidate window */
	if (move_window(fs, sect) != FR_OK) return 4;	/* Load boot record */

	if (ld_word(fs->win + BS_55AA) != 0xAA55){
		return 3;	/* Check boot record signature (always placed here even if the sector size is >512) */
	}

	if (fs->win[BS_JmpBoot] == 0xE9 || fs->win[BS_JmpBoot] == 0xEB || fs->win[BS_JmpBoot] == 0xE8) {	/* Valid JumpBoot code? */
		if (!memcmp(fs->win + BS_FilSysType, "FAT", 3)) return 0;		/* Is it an FAT VBR? */
		if (!memcmp(fs->win + BS_FilSysType32, "FAT32", 5)) return 0;	/* Is it an FAT32 VBR? */
	}
	return 2;	/* Valid BS but not FATFS */
}

FRESULT FatFs::find_volume(const TCHAR **path, FATFS **rfs, BYTE mode) {
	BYTE fmt, *pt;
	FATFS *fs;
	DWORD bsect, fasize, tsect, sysect, nclst, szbfat, br[4];
	WORD nrsv;
	UINT i;

	/* Get logical drive number */
	*rfs = 0;

	/* Check if the filesystem object is valid or not */
	fs = &FatVolume;					/* Get pointer to the filesystem object */
	if (!fs) return FR_NOT_ENABLED;		/* Is the filesystem object available? */
	*rfs = fs;							/* Return pointer to the filesystem object */

	mode &= (BYTE)~FA_READ;				/* Desired access mode, write access or not */
	if (fs->fs_type != 0) {				/* If the volume has been mounted */
		return FR_OK;				/* The filesystem object is valid */
	}

	/* The filesystem object is not valid. */
	/* Following code attempts to mount the volume. (analyze BPB and initialize the filesystem object) */

	fs->fs_type = 0;					/* Clear the filesystem object */

	/* Find an FAT partition on the drive. Supports only generic partitioning rules, FDISK and SFD. */
	bsect = 0;
	fmt = check_fs(fs, bsect);			/* Load sector 0 and check if it is an FAT-VBR as SFD */
	if (fmt == 2 || (fmt < 2 && LD2PT(vol) != 0)) {	/* Not an FAT-VBR or forced partition number */
		for (i = 0; i < 4; i++) {		/* Get partition offset */
			pt = fs->win + (MBR_Table + i * SZ_PTE);
			br[i] = pt[PTE_System] ? ld_dword(pt + PTE_StLba) : 0;
		}
		i = LD2PT(vol);					/* Partition number: 0:auto, 1-4:forced */
		if (i != 0) i--;
		do {							/* Find an FAT volume */
			bsect = br[i];
			fmt = bsect ? check_fs(fs, bsect) : 3;	/* Check the partition */
		} while (LD2PT(vol) == 0 && fmt >= 2 && ++i < 4);
	}
	if (fmt == 4) return FR_DISK_ERR;		/* An error occured in the disk I/O layer */
	if (fmt >= 2) return FR_NO_FILESYSTEM;	/* No FAT volume is found */

	{
		if (ld_word(fs->win + BPB_BytsPerSec) != SS(fs)) return FR_NO_FILESYSTEM;	/* (BPB_BytsPerSec must be equal to the physical sector size) */

		fasize = ld_word(fs->win + BPB_FATSz16);		/* Number of sectors per FATFS */
		if (fasize == 0) fasize = ld_dword(fs->win + BPB_FATSz32);
		fs->fsize = fasize;

		fs->n_fats = fs->win[BPB_NumFATs];				/* Number of FATs */
		if (fs->n_fats != 1 && fs->n_fats != 2) return FR_NO_FILESYSTEM;	/* (Must be 1 or 2) */
		fasize *= fs->n_fats;							/* Number of sectors for FAT area */

		fs->csize = fs->win[BPB_SecPerClus];			/* Cluster size */
		if (fs->csize == 0 || (fs->csize & (fs->csize - 1))) return FR_NO_FILESYSTEM;	/* (Must be power of 2) */

		fs->n_rootdir = ld_word(fs->win + BPB_RootEntCnt);	/* Number of root directory entries */
		if (fs->n_rootdir % (SS(fs) / SZDIRE)) return FR_NO_FILESYSTEM;	/* (Must be sector aligned) */

		tsect = ld_word(fs->win + BPB_TotSec16);		/* Number of sectors on the volume */
		if (tsect == 0) tsect = ld_dword(fs->win + BPB_TotSec32);

		nrsv = ld_word(fs->win + BPB_RsvdSecCnt);		/* Number of reserved sectors */
		if (nrsv == 0) return FR_NO_FILESYSTEM;			/* (Must not be 0) */

		/* Determine the FAT sub type */
		sysect = nrsv + fasize + fs->n_rootdir / (SS(fs) / SZDIRE);	/* RSV + FAT + DIR */
		if (tsect < sysect) return FR_NO_FILESYSTEM;	/* (Invalid volume size) */
		nclst = (tsect - sysect) / fs->csize;			/* Number of clusters */
		if (nclst == 0) return FR_NO_FILESYSTEM;		/* (Invalid volume size) */
		fmt = 0;
		if (nclst <= MAX_FAT32) fmt = FS_FAT32;
		if (nclst <= MAX_FAT16) fmt = FS_FAT16;
		if (nclst <= MAX_FAT12) fmt = FS_FAT12;
		if (fmt == 0) return FR_NO_FILESYSTEM;

		/* Boundaries and Limits */
		fs->n_fatent = nclst + 2;						/* Number of FAT entries */
		fs->volbase = bsect;							/* Volume start sector */
		fs->fatbase = bsect + nrsv; 					/* FAT start sector */
		fs->database = bsect + sysect;					/* Data start sector */
		if (fmt == FS_FAT32) {
			if (ld_word(fs->win + BPB_FSVer32) != 0) return FR_NO_FILESYSTEM;	/* (Must be FAT32 revision 0.0) */
			if (fs->n_rootdir != 0) return FR_NO_FILESYSTEM;	/* (BPB_RootEntCnt must be 0) */
			fs->dirbase = ld_dword(fs->win + BPB_RootClus32);	/* Root directory start cluster */
			szbfat = fs->n_fatent * 4;					/* (Needed FAT size) */
		} else {
			if (fs->n_rootdir == 0)	return FR_NO_FILESYSTEM;	/* (BPB_RootEntCnt must not be 0) */
			fs->dirbase = fs->fatbase + fasize;			/* Root directory start sector */
			szbfat = (fmt == FS_FAT16) ?				/* (Needed FAT size) */
				fs->n_fatent * 2 : fs->n_fatent * 3 / 2 + (fs->n_fatent & 1);
		}
		if (fs->fsize < (szbfat + (SS(fs) - 1)) / SS(fs)) return FR_NO_FILESYSTEM;	/* (BPB_FATSz must not be less than the size needed) */

		/* Get FSInfo if available */
		fs->last_clst = fs->free_clst = 0xFFFFFFFF;		/* Initialize cluster allocation information */
		fs->fsi_flag = 0x80;

		if (fmt == FS_FAT32				/* Allow to update FSInfo only if BPB_FSInfo32 == 1 */
			&& ld_word(fs->win + BPB_FSInfo32) == 1
			&& move_window(fs, bsect + 1) == FR_OK)
		{
			fs->fsi_flag = 0;
			if (ld_word(fs->win + BS_55AA) == 0xAA55	/* Load FSInfo data if available */
				&& ld_dword(fs->win + FSI_LeadSig) == 0x41615252
				&& ld_dword(fs->win + FSI_StrucSig) == 0x61417272)
			{
				fs->free_clst = ld_dword(fs->win + FSI_Free_Count);
				fs->last_clst = ld_dword(fs->win + FSI_Nxt_Free);
			}
		}
	}

	fs->fs_type = fmt;		/* FAT sub-type */
	fs->id = ++Fsid;		/* Volume mount ID */

	return FR_OK;
}

FRESULT FatFs::validate(FFOBJID *obj, FATFS **rfs) {
	FRESULT res = FR_INVALID_OBJECT;

	if (obj && obj->fs && obj->fs->fs_type && obj->id == obj->fs->id) {	/* Test if the object is valid */
		res = FR_OK;
	}
	*rfs = (res == FR_OK) ? obj->fs : 0;	/* Corresponding filesystem object */
	return res;
}

FRESULT FatFs::f_mount(StorageDevice *disk, BYTE opt) {
	FRESULT res;
	FATFS *fs = &FatVolume;

	fs->disk = disk;
	fs->fs_type = 0;						/* Clear old fs object */

	if (opt == 0) return FR_OK;					/* Do not mount now, it will be mounted later */

	char *name = (char *) fs->disk->getName();
	res = find_volume((const TCHAR **) &name, &fs, 0);	/* Force mounted the volume */
	LEAVE_FF(fs, res);
}

FRESULT FatFs::f_open(FIL *fp, const TCHAR *path, BYTE mode) {
	FRESULT res;
	DIR dj;
	FATFS *fs;
	DWORD dw, cl, bcs, clst, sc;
	FSIZE_t ofs;
	DEF_NAMBUF

	if (!fp) return FR_INVALID_OBJECT;

	/* Get logical drive */
	mode &= FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_CREATE_NEW | FA_OPEN_ALWAYS | FA_OPEN_APPEND;
	res = find_volume(&path, &fs, mode);
	if (res == FR_OK) {
		dj.obj.fs = fs;
		INIT_NAMBUF(fs);
		res = follow_path(&dj, path);	/* Follow the file path */

		if (res == FR_OK) {
			if (dj.fn[NSFLAG] & NS_NONAME) {	/* Origin directory itself? */
				res = FR_INVALID_NAME;
			}
		}
		/* Create or Open a file */
		if (mode & (FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW)) {
			if (res != FR_OK) {					/* No file, create new */
				if (res == FR_NO_FILE) {		/* There is no file to open, create a new entry */
					res = dir_register(&dj);
				}
				mode |= FA_CREATE_ALWAYS;		/* File is created */
			}
			else {								/* Any object with the same name is already existing */
				if (dj.obj.attr & (AM_RDO | AM_DIR)) {	/* Cannot overwrite it (R/O or DIR) */
					res = FR_DENIED;
				} else {
					if (mode & FA_CREATE_NEW) res = FR_EXIST;	/* Cannot create as new file */
				}
			}
			if (res == FR_OK && (mode & FA_CREATE_ALWAYS)) {	/* Truncate the file if overwrite mode */
				{
					/* Set directory entry initial state */
					cl = ld_clust(fs, dj.dir);			/* Get current cluster chain */
					st_dword(dj.dir + DIR_CrtTime, GET_FATTIME());	/* Set created time */
					dj.dir[DIR_Attr] = AM_ARC;			/* Reset attribute */
					st_clust(fs, dj.dir, 0);			/* Reset file allocation info */
					st_dword(dj.dir + DIR_FileSize, 0);
					fs->wflag = 1;
					if (cl != 0) {						/* Remove the cluster chain if exist */
						dw = fs->winsect;
						res = remove_chain(&dj.obj, cl, 0);
						if (res == FR_OK) {
							res = move_window(fs, dw);
							fs->last_clst = cl - 1;		/* Reuse the cluster hole */
						}
					}
				}
			}
		}
		else {	/* Open an existing file */
			if (res == FR_OK) {					/* Is the object exsiting? */
				if (dj.obj.attr & AM_DIR) {		/* File open against a directory */
					res = FR_NO_FILE;
				} else {
					if ((mode & FA_WRITE) && (dj.obj.attr & AM_RDO)) { /* Write mode open against R/O file */
						res = FR_DENIED;
					}
				}
			}
		}
		if (res == FR_OK) {
			if (mode & FA_CREATE_ALWAYS) mode |= FA_MODIFIED;	/* Set file change flag if created or overwritten */
			fp->dir_sect = fs->winsect;			/* Pointer to the directory entry */
			fp->dir_ptr = dj.dir;
		}

		if (res == FR_OK) {
			{
				fp->obj.sclust = ld_clust(fs, dj.dir);					/* Get object allocation info */
				fp->obj.objsize = ld_dword(dj.dir + DIR_FileSize);
			}
			fp->obj.fs = fs;	 	/* Validate the file object */
			fp->obj.id = fs->id;
			fp->flag = mode;		/* Set file access mode */
			fp->err = 0;			/* Clear error flag */
			fp->sect = 0;			/* Invalidate current data sector */
			fp->fptr = 0;			/* Set file pointer top of the file */
			
			memset(fp->buf, 0, FF_MAX_SS);	/* Clear sector buffer */
			if ((mode & FA_SEEKEND) && fp->obj.objsize > 0) {	/* Seek to end of file if FA_OPEN_APPEND is specified */
				fp->fptr = fp->obj.objsize;			/* Offset to seek */
				bcs = (DWORD)fs->csize * SS(fs);	/* Cluster size in byte */
				clst = fp->obj.sclust;				/* Follow the cluster chain */
				for (ofs = fp->obj.objsize; res == FR_OK && ofs > bcs; ofs -= bcs) {
					clst = get_fat(&fp->obj, clst);
					if (clst <= 1) res = FR_INT_ERR;
					if (clst == 0xFFFFFFFF) res = FR_DISK_ERR;
				}
				fp->clust = clst;
				if (res == FR_OK && ofs % SS(fs)) {	/* Fill sector buffer if not on the sector boundary */
					if ((sc = clst2sect(fs, clst)) == 0) {
						res = FR_INT_ERR;
					} else {
						fp->sect = sc + (DWORD)(ofs / SS(fs));
						if (!fs->disk->read(fp->buf, fp->sect, 1)) res = FR_DISK_ERR;
					}
				}
			}
		}

		FREE_NAMBUF();
	}

	if (res != FR_OK) fp->obj.fs = 0;	/* Invalidate file object on error */

	LEAVE_FF(fs, res);
}

FRESULT FatFs::f_read (FIL* fp, void* buff, UINT btr, UINT* br) {
	FRESULT res;
	FATFS *fs;
	DWORD clst, sect;
	FSIZE_t remain;
	UINT rcnt, cc, csect;
	BYTE *rbuff = (BYTE*)buff;

	*br = 0;	/* Clear read byte counter */
	res = validate(&fp->obj, &fs);				/* Check validity of the file object */
	if (res != FR_OK || (res = (FRESULT)fp->err) != FR_OK) LEAVE_FF(fs, res);	/* Check validity */
	if (!(fp->flag & FA_READ)) LEAVE_FF(fs, FR_DENIED); /* Check access mode */
	remain = fp->obj.objsize - fp->fptr;
	if (btr > remain) btr = (UINT)remain;		/* Truncate btr by remaining bytes */

	for ( ;  btr;								/* Repeat until all data read */
		btr -= rcnt, *br += rcnt, rbuff += rcnt, fp->fptr += rcnt) {
		if (fp->fptr % SS(fs) == 0) {			/* On the sector boundary? */
			csect = (UINT)(fp->fptr / SS(fs) & (fs->csize - 1));	/* Sector offset in the cluster */
			if (csect == 0) {					/* On the cluster boundary? */
				if (fp->fptr == 0) {			/* On the top of the file? */
					clst = fp->obj.sclust;		/* Follow cluster chain from the origin */
				} else {						/* Middle or end of the file */
					{
						clst = get_fat(&fp->obj, fp->clust);	/* Follow cluster chain on the FATFS */
					}
				}
				if (clst < 2) ABORT(fs, FR_INT_ERR);
				if (clst == 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR);
				fp->clust = clst;				/* Update current cluster */
			}
			sect = clst2sect(fs, fp->clust);	/* Get current sector */
			if (sect == 0) ABORT(fs, FR_INT_ERR);
			sect += csect;
			cc = btr / SS(fs);					/* When remaining bytes >= sector size, */
			if (cc > 0) {						/* Read maximum contiguous sectors directly */
				if (csect + cc > fs->csize) {	/* Clip at cluster boundary */
					cc = fs->csize - csect;
				}
				if (!fs->disk->read(rbuff, sect, cc)) ABORT(fs, FR_DISK_ERR);
				if ((fp->flag & FA_DIRTY) && fp->sect - sect < cc) {
					memcpy(rbuff + ((fp->sect - sect) * SS(fs)), fp->buf, SS(fs));
				}
				rcnt = SS(fs) * cc;				/* Number of bytes transferred */
				continue;
			}
			if (fp->sect != sect) {			/* Load data sector if not in cache */
				if (fp->flag & FA_DIRTY) {		/* Write-back dirty sector cache */
					if (!fs->disk->write(fp->buf, fp->sect, 1)) ABORT(fs, FR_DISK_ERR);
					fp->flag &= (BYTE)~FA_DIRTY;
				}
				if (!fs->disk->read(fp->buf, sect, 1))	ABORT(fs, FR_DISK_ERR);	/* Fill sector cache */
			}
			fp->sect = sect;
		}
		rcnt = SS(fs) - (UINT)fp->fptr % SS(fs);	/* Number of bytes left in the sector */
		if (rcnt > btr) rcnt = btr;					/* Clip it by btr if needed */
		memcpy(rbuff, fp->buf + fp->fptr % SS(fs), rcnt);	/* Extract partial sector */
	}

	LEAVE_FF(fs, FR_OK);
}

FRESULT FatFs::f_write(FIL *fp, const void *buff, UINT btw, UINT *bw) {
	FRESULT res;
	FATFS *fs;
	DWORD clst, sect;
	UINT wcnt, cc, csect;
	const BYTE *wbuff = (const BYTE*)buff;


	*bw = 0;	/* Clear write byte counter */
	res = validate(&fp->obj, &fs);			/* Check validity of the file object */
	if (res != FR_OK || (res = (FRESULT)fp->err) != FR_OK) LEAVE_FF(fs, res);	/* Check validity */
	if (!(fp->flag & FA_WRITE)) LEAVE_FF(fs, FR_DENIED);	/* Check access mode */

	/* Check fptr wrap-around (file size cannot reach 4 GiB at FAT volume) */
	if ((fs->fs_type != FS_EXFAT) && (DWORD)(fp->fptr + btw) < (DWORD)fp->fptr) {
		btw = (UINT)(0xFFFFFFFF - (DWORD)fp->fptr);
	}

	for ( ;  btw;							/* Repeat until all data written */
		btw -= wcnt, *bw += wcnt, wbuff += wcnt, fp->fptr += wcnt, fp->obj.objsize = (fp->fptr > fp->obj.objsize) ? fp->fptr : fp->obj.objsize) {
		if (fp->fptr % SS(fs) == 0) {		/* On the sector boundary? */
			csect = (UINT)(fp->fptr / SS(fs)) & (fs->csize - 1);	/* Sector offset in the cluster */
			if (csect == 0) {				/* On the cluster boundary? */
				if (fp->fptr == 0) {		/* On the top of the file? */
					clst = fp->obj.sclust;	/* Follow from the origin */
					if (clst == 0) {		/* If no cluster is allocated, */
						clst = create_chain(&fp->obj, 0);	/* create a new cluster chain */
					}
				} else {					/* On the middle or end of the file */
					{
						clst = create_chain(&fp->obj, fp->clust);	/* Follow or stretch cluster chain on the FATFS */
					}
				}
				if (clst == 0) break;		/* Could not allocate a new cluster (disk full) */
				if (clst == 1) ABORT(fs, FR_INT_ERR);
				if (clst == 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR);
				fp->clust = clst;			/* Update current cluster */
				if (fp->obj.sclust == 0) fp->obj.sclust = clst;	/* Set start cluster if the first write */
			}
			if (fp->flag & FA_DIRTY) {		/* Write-back sector cache */
				if (!fs->disk->write(fp->buf, fp->sect, 1)) ABORT(fs, FR_DISK_ERR);
				fp->flag &= (BYTE)~FA_DIRTY;
			}
			sect = clst2sect(fs, fp->clust);	/* Get current sector */
			if (sect == 0) ABORT(fs, FR_INT_ERR);
			sect += csect;
			cc = btw / SS(fs);				/* When remaining bytes >= sector size, */
			if (cc > 0) {					/* Write maximum contiguous sectors directly */
				if (csect + cc > fs->csize) {	/* Clip at cluster boundary */
					cc = fs->csize - csect;
				}
				if (!fs->disk->write(wbuff, sect, cc)) ABORT(fs, FR_DISK_ERR);
				if (fp->sect - sect < cc) { /* Refill sector cache if it gets invalidated by the direct write */
					memcpy(fp->buf, wbuff + ((fp->sect - sect) * SS(fs)), SS(fs));
					fp->flag &= (BYTE)~FA_DIRTY;
				}
				wcnt = SS(fs) * cc;		/* Number of bytes transferred */
				continue;
			}
			if (fp->sect != sect && 		/* Fill sector cache with file data */
				fp->fptr < fp->obj.objsize &&
				!fs->disk->read(fp->buf, sect, 1)) {
					ABORT(fs, FR_DISK_ERR);
			}
			fp->sect = sect;
		}
		wcnt = SS(fs) - (UINT)fp->fptr % SS(fs);	/* Number of bytes left in the sector */
		if (wcnt > btw) wcnt = btw;					/* Clip it by btw if needed */
		memcpy(fp->buf + fp->fptr % SS(fs), wbuff, wcnt);	/* Fit data to the sector */
		fp->flag |= FA_DIRTY;
	}

	fp->flag |= FA_MODIFIED;				/* Set file change flag */

	LEAVE_FF(fs, FR_OK);
}

FRESULT FatFs::f_sync(FIL *fp) {
	FRESULT res;
	FATFS *fs;
	DWORD tm;
	BYTE *dir;

	res = validate(&fp->obj, &fs);	/* Check validity of the file object */
	if (res == FR_OK) {
		if (fp->flag & FA_MODIFIED) {	/* Is there any change to the file? */
			if (fp->flag & FA_DIRTY) {	/* Write-back cached data if needed */
				if (!fs->disk->write(fp->buf, fp->sect, 1)) LEAVE_FF(fs, FR_DISK_ERR);
				fp->flag &= (BYTE)~FA_DIRTY;
			}
			
			/* Update the directory entry */
			tm = GET_FATTIME();				/* Modified time */
			{
				res = move_window(fs, fp->dir_sect);
				if (res == FR_OK) {
					dir = fp->dir_ptr;
					dir[DIR_Attr] |= AM_ARC;						/* Set archive attribute to indicate that the file has been changed */
					st_clust(fp->obj.fs, dir, fp->obj.sclust);		/* Update file allocation information  */
					st_dword(dir + DIR_FileSize, (DWORD)fp->obj.objsize);	/* Update file size */
					st_dword(dir + DIR_ModTime, tm);				/* Update modified time */
					st_word(dir + DIR_LstAccDate, 0);
					fs->wflag = 1;
					res = sync_fs(fs);					/* Restore it to the directory */
					fp->flag &= (BYTE)~FA_MODIFIED;
				}
			}
		}
	}

	LEAVE_FF(fs, res);
}

FRESULT FatFs::f_close(FIL *fp) {
	FRESULT res;
	FATFS *fs;

	res = f_sync(fp);					/* Flush cached data */
	if (res == FR_OK)
	{
		res = validate(&fp->obj, &fs);	/* Lock volume */
		if (res == FR_OK) {
			fp->obj.fs = 0;	/* Invalidate file object */
		}
	}
	return res;
}

FRESULT FatFs::f_lseek(FIL *fp, FSIZE_t ofs) {
	FRESULT res;
	FATFS *fs;
	DWORD clst, bcs, nsect;
	FSIZE_t ifptr;

	res = validate(&fp->obj, &fs);		/* Check validity of the file object */
	if (res == FR_OK) res = (FRESULT)fp->err;
	if (res != FR_OK) LEAVE_FF(fs, res);

	/* Normal Seek */
	{
		if (ofs > fp->obj.objsize && (!(fp->flag & FA_WRITE))) {	/* In read-only mode, clip offset with the file size */
			ofs = fp->obj.objsize;
		}
		ifptr = fp->fptr;
		fp->fptr = nsect = 0;
		if (ofs > 0) {
			bcs = (DWORD)fs->csize * SS(fs);	/* Cluster size (byte) */
			if (ifptr > 0 &&
				(ofs - 1) / bcs >= (ifptr - 1) / bcs) {	/* When seek to same or following cluster, */
				fp->fptr = (ifptr - 1) & ~(FSIZE_t)(bcs - 1);	/* start from the current cluster */
				ofs -= fp->fptr;
				clst = fp->clust;
			} else {									/* When seek to back cluster, */
				clst = fp->obj.sclust;					/* start from the first cluster */
				if (clst == 0) {						/* If no cluster chain, create a new chain */
					clst = create_chain(&fp->obj, 0);
					if (clst == 1) ABORT(fs, FR_INT_ERR);
					if (clst == 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR);
					fp->obj.sclust = clst;
				}
				fp->clust = clst;
			}
			if (clst != 0) {
				while (ofs > bcs) {						/* Cluster following loop */
					ofs -= bcs; fp->fptr += bcs;
					if (fp->flag & FA_WRITE) {			/* Check if in write mode or not */
						clst = create_chain(&fp->obj, clst);	/* Follow chain with forceed stretch */
						if (clst == 0) {				/* Clip file size in case of disk full */
							ofs = 0; break;
						}
					} else
					{
						clst = get_fat(&fp->obj, clst);	/* Follow cluster chain if not in write mode */
					}
					if (clst == 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR);
					if (clst <= 1 || clst >= fs->n_fatent) ABORT(fs, FR_INT_ERR);
					fp->clust = clst;
				}
				fp->fptr += ofs;
				if (ofs % SS(fs)) {
					nsect = clst2sect(fs, clst);	/* Current sector */
					if (nsect == 0) ABORT(fs, FR_INT_ERR);
					nsect += (DWORD)(ofs / SS(fs));
				}
			}
		}
		if (fp->fptr > fp->obj.objsize) {	/* Set file change flag if the file size is extended */
			fp->obj.objsize = fp->fptr;
			fp->flag |= FA_MODIFIED;
		}
		if (fp->fptr % SS(fs) && nsect != fp->sect) {	/* Fill sector cache if needed */
			if (fp->flag & FA_DIRTY) {			/* Write-back dirty sector cache */
				if (!fs->disk->write(fp->buf, fp->sect, 1)) ABORT(fs, FR_DISK_ERR);
				fp->flag &= (BYTE)~FA_DIRTY;
			}
			if (!fs->disk->read(fp->buf, nsect, 1)) ABORT(fs, FR_DISK_ERR);	/* Fill sector cache */
			fp->sect = nsect;
		}
	}

	LEAVE_FF(fs, res);
}

FRESULT FatFs::f_opendir(DIR *dp, const TCHAR *path) {
	FRESULT res;
	FATFS *fs;
	DEF_NAMBUF

	if (!dp) return FR_INVALID_OBJECT;

	/* Get logical drive */
	res = find_volume(&path, &fs, 0);
	if (res == FR_OK) {
		dp->obj.fs = fs;
		INIT_NAMBUF(fs);
		res = follow_path(dp, path);			/* Follow the path to the directory */
		if (res == FR_OK) {						/* Follow completed */
			if (!(dp->fn[NSFLAG] & NS_NONAME)) {	/* It is not the origin directory itself */
				if (dp->obj.attr & AM_DIR) {		/* This object is a sub-directory */
					{
						dp->obj.sclust = ld_clust(fs, dp->dir);	/* Get object allocation info */
					}
				} else {						/* This object is a file */
					res = FR_NO_PATH;
				}
			}
			if (res == FR_OK) {
				dp->obj.id = fs->id;
				res = dir_sdi(dp, 0);			/* Rewind directory */
			}
		}
		FREE_NAMBUF();
		if (res == FR_NO_FILE) res = FR_NO_PATH;
	}
	if (res != FR_OK) dp->obj.fs = 0;		/* Invalidate the directory object if function faild */

	LEAVE_FF(fs, res);
}

FRESULT FatFs::f_closedir(DIR *dp) {
	FRESULT res;
	FATFS *fs;

	res = validate(&dp->obj, &fs);	/* Check validity of the file object */
	if (res == FR_OK) {
		dp->obj.fs = 0;	/* Invalidate directory object */
	}

	return res;
}

FRESULT FatFs::f_readdir(DIR *dp, FILINFO *fno) {
	FRESULT res;
	FATFS *fs;
	DEF_NAMBUF

	res = validate(&dp->obj, &fs);	/* Check validity of the directory object */
	if (res == FR_OK) {
		if (!fno) {
			res = dir_sdi(dp, 0);			/* Rewind the directory object */
		} else {
			INIT_NAMBUF(fs);
			res = dir_read_file(dp);		/* Read an item */
			if (res == FR_NO_FILE) res = FR_OK;	/* Ignore end of directory */
			if (res == FR_OK) {				/* A valid entry is found */
				get_fileinfo(dp, fno);		/* Get the object information */
				res = dir_next(dp, 0);		/* Increment index for next */
				if (res == FR_NO_FILE) res = FR_OK;	/* Ignore end of directory now */
			}
			FREE_NAMBUF();
		}
	}

	LEAVE_FF(fs, res);
}

FRESULT FatFs::f_stat(const TCHAR *path, FILINFO *fno) {
	FRESULT res;
	DIR dj;
	DEF_NAMBUF

	/* Get logical drive */
	res = find_volume(&path, &dj.obj.fs, 0);
	if (res == FR_OK) {
		INIT_NAMBUF(dj.obj.fs);
		res = follow_path(&dj, path);	/* Follow the file path */
		if (res == FR_OK) {				/* Follow completed */
			if (dj.fn[NSFLAG] & NS_NONAME) {	/* It is origin directory */
				res = FR_INVALID_NAME;
			} else {							/* Found an object */
				if (fno) get_fileinfo(&dj, fno);
			}
		}
		FREE_NAMBUF();
	}

	LEAVE_FF(dj.obj.fs, res);
}

FRESULT FatFs::f_getfree(const TCHAR *path, DWORD *nclst, FATFS **Fat) {
	FRESULT res;
	FATFS *fs;
	DWORD nfree, clst, sect, stat;
	UINT i;
	FFOBJID obj;

	/* Get logical drive */
	res = find_volume(&path, &fs, 0);
	if (res == FR_OK) {
		*Fat = fs;				/* Return ptr to the fs object */
		/* If free_clst is valid, return it without full FAT scan */
		if (fs->free_clst <= fs->n_fatent - 2) {
			*nclst = fs->free_clst;
		} else {
			/* Scan FAT to obtain number of free clusters */
			nfree = 0;
			if (fs->fs_type == FS_FAT12) {	/* FAT12: Scan bit field FAT entries */
				clst = 2; obj.fs = fs;
				do {
					stat = get_fat(&obj, clst);
					if (stat == 0xFFFFFFFF) { res = FR_DISK_ERR; break; }
					if (stat == 1) { res = FR_INT_ERR; break; }
					if (stat == 0) nfree++;
				} while (++clst < fs->n_fatent);
			} else {
				{	/* FAT16/32: Scan WORD/DWORD FAT entries */
					clst = fs->n_fatent;	/* Number of entries */
					sect = fs->fatbase;		/* Top of the FATFS */
					i = 0;					/* Offset in the sector */
					do {	/* Counts numbuer of entries with zero in the FATFS */
						if (i == 0) {
							res = move_window(fs, sect++);
							if (res != FR_OK) break;
						}
						if (fs->fs_type == FS_FAT16) {
							if (ld_word(fs->win + i) == 0) nfree++;
							i += 2;
						} else {
							if ((ld_dword(fs->win + i) & 0x0FFFFFFF) == 0) nfree++;
							i += 4;
						}
						i %= SS(fs);
					} while (--clst);
				}
			}
			*nclst = nfree;			/* Return the free clusters */
			fs->free_clst = nfree;	/* Now free_clst is valid */
			fs->fsi_flag |= 1;		/* FAT32: FSInfo is to be updated */
		}
	}

	LEAVE_FF(fs, res);
}

FRESULT FatFs::f_truncate(FIL *fp) {
	FRESULT res;
	FATFS *fs;
	DWORD ncl;

	res = validate(&fp->obj, &fs);	/* Check validity of the file object */
	if (res != FR_OK || (res = (FRESULT)fp->err) != FR_OK) LEAVE_FF(fs, res);
	if (!(fp->flag & FA_WRITE)) LEAVE_FF(fs, FR_DENIED);	/* Check access mode */

	if (fp->fptr < fp->obj.objsize) {	/* Process when fptr is not on the eof */
		if (fp->fptr == 0) {	/* When set file size to zero, remove entire cluster chain */
			res = remove_chain(&fp->obj, fp->obj.sclust, 0);
			fp->obj.sclust = 0;
		} else {				/* When truncate a part of the file, remove remaining clusters */
			ncl = get_fat(&fp->obj, fp->clust);
			res = FR_OK;
			if (ncl == 0xFFFFFFFF) res = FR_DISK_ERR;
			if (ncl == 1) res = FR_INT_ERR;
			if (res == FR_OK && ncl < fs->n_fatent) {
				res = remove_chain(&fp->obj, ncl, fp->clust);
			}
		}
		fp->obj.objsize = fp->fptr;	/* Set file size to current read/write point */
		fp->flag |= FA_MODIFIED;

		if (res == FR_OK && (fp->flag & FA_DIRTY)) {
			if (!fs->disk->write(fp->buf, fp->sect, 1)) {
				res = FR_DISK_ERR;
			} else {
				fp->flag &= (BYTE)~FA_DIRTY;
			}
		}

		if (res != FR_OK) ABORT(fs, res);
	}

	LEAVE_FF(fs, res);
}

FRESULT FatFs::f_unlink(const TCHAR *path) {
	FRESULT res;
	DIR dj, sdj;
	DWORD dclst = 0;
	FATFS *fs;
	DEF_NAMBUF

	/* Get logical drive */
	res = find_volume(&path, &fs, FA_WRITE);
	if (res == FR_OK) {
		dj.obj.fs = fs;
		INIT_NAMBUF(fs);
		res = follow_path(&dj, path);		/* Follow the file path */
		if (res == FR_OK) {					/* The object is accessible */
			if (dj.fn[NSFLAG] & NS_NONAME) {
				res = FR_INVALID_NAME;		/* Cannot remove the origin directory */
			} else {
				if (dj.obj.attr & AM_RDO) {
					res = FR_DENIED;		/* Cannot remove R/O object */
				}
			}
			if (res == FR_OK) {
				{
					dclst = ld_clust(fs, dj.dir);
				}
				if (dj.obj.attr & AM_DIR) {			/* Is it a sub-directory? */
					{
						sdj.obj.fs = fs;				/* Open the sub-directory */
						sdj.obj.sclust = dclst;
						res = dir_sdi(&sdj, 0);
						if (res == FR_OK) {
							res = dir_read_file(&sdj);			/* Test if the directory is empty */
							if (res == FR_OK) res = FR_DENIED;	/* Not empty? */
							if (res == FR_NO_FILE) res = FR_OK;	/* Empty? */
						}
					}
				}
			}
			if (res == FR_OK) {
				res = dir_remove(&dj);			/* Remove the directory entry */
				if (res == FR_OK && dclst != 0) {	/* Remove the cluster chain if exist */
					res = remove_chain(&dj.obj, dclst, 0);
				}
				if (res == FR_OK) res = sync_fs(fs);
			}
		}
		FREE_NAMBUF();
	}

	LEAVE_FF(fs, res);
}

FRESULT FatFs::f_mkdir(const TCHAR *path) {
	FRESULT res;
	DIR dj;
	FATFS *fs;
	BYTE *dir;
	DWORD dcl, pcl, tm;
	DEF_NAMBUF

	/* Get logical drive */
	res = find_volume(&path, &fs, FA_WRITE);
	if (res == FR_OK) {
		dj.obj.fs = fs;
		INIT_NAMBUF(fs);
		res = follow_path(&dj, path);			/* Follow the file path */
		if (res == FR_OK) res = FR_EXIST;		/* Any object with same name is already existing */
		if (res == FR_NO_FILE) {				/* Can create a new directory */
			dcl = create_chain(&dj.obj, 0);		/* Allocate a cluster for the new directory table */
			dj.obj.objsize = (DWORD)fs->csize * SS(fs);
			res = FR_OK;
			if (dcl == 0) res = FR_DENIED;		/* No space to allocate a new cluster */
			if (dcl == 1) res = FR_INT_ERR;
			if (dcl == 0xFFFFFFFF) res = FR_DISK_ERR;
			if (res == FR_OK) res = sync_window(fs);	/* Flush FATFS */
			tm = GET_FATTIME();
			if (res == FR_OK) {					/* Initialize the new directory table */
				res = dir_clear(fs, dcl);		/* Clean up the new table */
				if (res == FR_OK && (fs->fs_type != FS_EXFAT)) {	/* Create dot entries (FAT only) */
					dir = fs->win;
					memset(dir + DIR_Name, ' ', 11);	/* Create "." entry */
					dir[DIR_Name] = '.';
					dir[DIR_Attr] = AM_DIR;
					st_dword(dir + DIR_ModTime, tm);
					st_clust(fs, dir, dcl);
					memcpy(dir + SZDIRE, dir, SZDIRE); /* Create ".." entry */
					dir[SZDIRE + 1] = '.'; pcl = dj.obj.sclust;
					st_clust(fs, dir + SZDIRE, pcl);
					fs->wflag = 1;
				}
			}
			if (res == FR_OK) {
				res = dir_register(&dj);	/* Register the object to the directoy */
			}
			if (res == FR_OK) {
				{
					dir = dj.dir;
					st_dword(dir + DIR_ModTime, tm);	/* Created time */
					st_clust(fs, dir, dcl);				/* Table start cluster */
					dir[DIR_Attr] = AM_DIR;				/* Attribute */
					fs->wflag = 1;
				}
				if (res == FR_OK) {
					res = sync_fs(fs);
				}
			} else {
				remove_chain(&dj.obj, dcl, 0);		/* Could not register, remove cluster chain */
			}
		}
		FREE_NAMBUF();
	}

	LEAVE_FF(fs, res);
}

FRESULT FatFs::f_rename(const TCHAR *path_old, const TCHAR *path_new) {
	FRESULT res;
	DIR djo, djn;
	FATFS *fs;
	BYTE buf[SZDIRE], *dir;
	DWORD dw;
	DEF_NAMBUF

	//get_ldnumber(&path_new);						/* Snip the drive number of new name off */
	res = find_volume(&path_old, &fs, FA_WRITE);	/* Get logical drive of the old object */
	if (res == FR_OK) {
		djo.obj.fs = fs;
		INIT_NAMBUF(fs);
		res = follow_path(&djo, path_old);		/* Check old object */
		if (res == FR_OK && (djo.fn[NSFLAG] & (NS_DOT | NS_NONAME))) res = FR_INVALID_NAME;	/* Check validity of name */
		if (res == FR_OK) {						/* Object to be renamed is found */
			{	/* At FAT/FAT32 volume */
				memcpy(buf, djo.dir, SZDIRE);			/* Save directory entry of the object */
				memcpy(&djn, &djo, sizeof (DIR));		/* Duplicate the directory object */
				res = follow_path(&djn, path_new);		/* Make sure if new object name is not in use */
				if (res == FR_OK) {						/* Is new name already in use by any other object? */
					res = (djn.obj.sclust == djo.obj.sclust && djn.dptr == djo.dptr) ? FR_NO_FILE : FR_EXIST;
				}
				if (res == FR_NO_FILE) { 				/* It is a valid path and no name collision */
					res = dir_register(&djn);			/* Register the new entry */
					if (res == FR_OK) {
						dir = djn.dir;					/* Copy directory entry of the object except name */
						memcpy(dir + 13, buf + 13, SZDIRE - 13);
						dir[DIR_Attr] = buf[DIR_Attr];
						if (!(dir[DIR_Attr] & AM_DIR)) dir[DIR_Attr] |= AM_ARC;	/* Set archive attribute if it is a file */
						fs->wflag = 1;
						if ((dir[DIR_Attr] & AM_DIR) && djo.obj.sclust != djn.obj.sclust) {	/* Update .. entry in the sub-directory if needed */
							dw = clst2sect(fs, ld_clust(fs, dir));
							if (dw == 0) {
								res = FR_INT_ERR;
							} else {
/* Start of critical section where an interruption can cause a cross-link */
								res = move_window(fs, dw);
								dir = fs->win + SZDIRE * 1;	/* Ptr to .. entry */
								if (res == FR_OK && dir[1] == '.') {
									st_clust(fs, dir, djn.obj.sclust);
									fs->wflag = 1;
								}
							}
						}
					}
				}
			}
			if (res == FR_OK) {
				res = dir_remove(&djo);		/* Remove old entry */
				if (res == FR_OK) {
					res = sync_fs(fs);
				}
			}
/* End of the critical section */
		}
		FREE_NAMBUF();
	}

	LEAVE_FF(fs, res);
}

FRESULT FatFs::f_getlabel(const TCHAR *path, TCHAR *label, DWORD *vsn) {
	FRESULT res;
	DIR dj;
	FATFS *fs;
	UINT si, di;
	WCHAR wc;

	/* Get logical drive */
	res = find_volume(&path, &fs, 0);

	/* Get volume label */
	if (res == FR_OK && label) {
		dj.obj.fs = fs; dj.obj.sclust = 0;	/* Open root directory */
		res = dir_sdi(&dj, 0);
		if (res == FR_OK) {
		 	res = dir_read_label(&dj);		/* Find a volume label entry */
		 	if (res == FR_OK) {
				{
					si = di = 0;		/* Extract volume label from AM_VOL entry */
					while (si < 11) {
						wc = dj.dir[si++];							/* ANSI/OEM output */
						label[di++] = (TCHAR)wc;
					}
					do {				/* Truncate trailing spaces */
						label[di] = 0;
						if (di == 0) break;
					} while (label[--di] == ' ');
				}
			}
		}
		if (res == FR_NO_FILE) {	/* No label entry and return nul string */
			label[0] = 0;
			res = FR_OK;
		}
	}

	/* Get volume serial number */
	if (res == FR_OK && vsn) {
		res = move_window(fs, fs->volbase);
		if (res == FR_OK) {
			switch (fs->fs_type) {
			case FS_EXFAT:
				di = BPB_VolIDEx; break;

			case FS_FAT32:
				di = BS_VolID32; break;

			default:
				di = BS_VolID;
			}
			*vsn = ld_dword(fs->win + di);
		}
	}

	LEAVE_FF(fs, res);
}

FRESULT FatFs::f_setlabel(const TCHAR *label) {
	FRESULT res;
	DIR dj;
	FATFS *fs;
	BYTE dirvn[22];
	UINT di;
	WCHAR wc;
	static const char badchr[] = "+.,;=[]\"*:<>\?|\x7F";	/* [0..] for FAT, [7..] for exFATFS */
	DWORD dc;

	/* Get logical drive */
	res = find_volume(&label, &fs, FA_WRITE);
	if (res != FR_OK) LEAVE_FF(fs, res);

	{	/* On the FAT/FAT32 volume */
		memset(dirvn, ' ', 11);
		di = 0;
		while (*label) {	/* Create volume label in directory form */
			dc = tchar2uni(&label);
			wc = (dc < 0x10000) ? ff_uni2oem(ff_wtoupper(dc), FF_CODE_PAGE) : 0;
			if (wc >= 0x80) wc = ExCvt[wc - 0x80];	/* To upper extended characters (SBCS cfg) */
			if (wc == 0 || strchr(badchr + 0, (int)wc) || di >= (UINT)((wc >= 0x100) ? 10 : 11)) {	/* Reject invalid characters for volume label */
				LEAVE_FF(fs, FR_INVALID_NAME);
			}
			if (wc >= 0x100) dirvn[di++] = (BYTE)(wc >> 8);
			dirvn[di++] = (BYTE)wc;
		}
		if (dirvn[0] == DDEM) LEAVE_FF(fs, FR_INVALID_NAME);	/* Reject illegal name (heading DDEM) */
		while (di && dirvn[di - 1] == ' ') di--;				/* Snip trailing spaces */
	}

	/* Set volume label */
	dj.obj.fs = fs; dj.obj.sclust = 0;	/* Open root directory */
	res = dir_sdi(&dj, 0);
	if (res == FR_OK) {
		res = dir_read_label(&dj);	/* Get volume label entry */
		if (res == FR_OK) {
			if (fs->fs_type == FS_EXFAT) {
				dj.dir[XDIR_NumLabel] = (BYTE)di;	/* Change the volume label */
				memcpy(dj.dir + XDIR_Label, dirvn, 22);
			} else {
				if (di != 0) {
					memcpy(dj.dir, dirvn, 11);	/* Change the volume label */
				} else {
					dj.dir[DIR_Name] = DDEM;	/* Remove the volume label */
				}
			}
			fs->wflag = 1;
			res = sync_fs(fs);
		} else {			/* No volume label entry or an error */
			if (res == FR_NO_FILE) {
				res = FR_OK;
				if (di != 0) {	/* Create a volume label entry */
					res = dir_alloc(&dj, 1);	/* Allocate an entry */
					if (res == FR_OK) {
						memset(dj.dir, 0, SZDIRE);	/* Clean the entry */
						if (fs->fs_type == FS_EXFAT) {
							dj.dir[XDIR_Type] = 0x83;		/* Create 83 entry */
							dj.dir[XDIR_NumLabel] = (BYTE)di;
							memcpy(dj.dir + XDIR_Label, dirvn, 22);
						} else {
							dj.dir[DIR_Attr] = AM_VOL;		/* Create volume label entry */
							memcpy(dj.dir, dirvn, 11);
						}
						fs->wflag = 1;
						res = sync_fs(fs);
					}
				}
			}
		}
	}

	LEAVE_FF(fs, res);
}

FRESULT FatFs::f_mkfs (BYTE opt, DWORD au, void *work, UINT len) {
	const UINT n_fats = 1;		/* Number of FATs for FAT/FAT32 volume (1 or 2) */
	const UINT n_rootdir = 512;	/* Number of root directory entries for FAT volume */
	static const WORD cst[] = {1, 4, 16, 64, 256, 512, 0};	/* Cluster size boundary for FAT volume (4Ks unit) */
	static const WORD cst32[] = {1, 2, 4, 8, 16, 32, 0};	/* Cluster size boundary for FAT32 volume (128Ks unit) */
	BYTE fmt, *buf;
	WORD ss;	/* Sector size */
	DWORD szb_buf, sz_buf, sz_blk, n_clst, pau, sect, nsect, n;
	DWORD b_vol, b_fat, b_data;				/* Base LBA for volume, fat, data */
	DWORD sz_vol, sz_rsv, sz_fat, sz_dir;	/* Size for volume, fat, dir, data */
	UINT i;

	/* Check mounted drive and clear work area */
	if (FatVolume.fs_type) FatVolume.fs_type = 0;	/* Clear the volume if mounted */
	sz_blk = 1;

	ss = FF_MAX_SS;
	if ((au != 0 && au < ss) || au > 0x1000000 || (au & (au - 1))) return FR_INVALID_PARAMETER;	/* Check if au is valid */
	au /= ss;	/* Cluster size in unit of sector */

	if (!work) {	/* Use heap memory for working buffer */
		for (szb_buf = MAX_MALLOC, buf = 0; szb_buf >= ss && !(buf = (BYTE *) malloc(szb_buf)); szb_buf /= 2) ;
		sz_buf = szb_buf / ss;		/* Size of working buffer (sector) */
	} else {
		buf = (BYTE*)work;		/* Working buffer */
		sz_buf = len / ss;		/* Size of working buffer (sector) */
		szb_buf = sz_buf * ss;	/* Size of working buffer (byte) */
	}
	if (!buf || sz_buf == 0) return FR_NOT_ENOUGH_CORE;

	/* Create a single-partition in this function */
	sz_vol = FatVolume.disk->getSectorCount();
	b_vol = 0;		/* Volume start sector */
	if (sz_vol < 128) LEAVE_MKFS(FR_MKFS_ABORTED);	/* Check if volume size is >=128s */

	/* Pre-determine the FAT type */
	do {
		if (au > 128) LEAVE_MKFS(FR_INVALID_PARAMETER);	/* Too large au for FAT/FAT32 */
		if (opt & FM_FAT32) {	/* FAT32 possible? */
			if ((opt & FM_ANY) == FM_FAT32 || !(opt & FM_FAT)) {	/* FAT32 only or no-FAT? */
				fmt = FS_FAT32; break;
			}
		}
		if (!(opt & FM_FAT)) LEAVE_MKFS(FR_INVALID_PARAMETER);	/* no-FAT? */
		fmt = FS_FAT16;
	} while (0);

	/* Create an FAT/FAT32 volume */
	do {
		pau = au;
		/* Pre-determine number of clusters and FAT sub-type */
		if (fmt == FS_FAT32) {	/* FAT32 volume */
			if (pau == 0) {	/* au auto-selection */
				n = sz_vol / 0x20000;	/* Volume size in unit of 128KS */
				for (i = 0, pau = 1; cst32[i] && cst32[i] <= n; i++, pau <<= 1) ;	/* Get from table */
			}
			n_clst = sz_vol / pau;	/* Number of clusters */
			sz_fat = (n_clst * 4 + 8 + ss - 1) / ss;	/* FAT size [sector] */
			sz_rsv = 32;	/* Number of reserved sectors */
			sz_dir = 0;		/* No static directory */
			if (n_clst <= MAX_FAT16 || n_clst > MAX_FAT32) LEAVE_MKFS(FR_MKFS_ABORTED);
		} else {				/* FAT volume */
			if (pau == 0) {	/* au auto-selection */
				n = sz_vol / 0x1000;	/* Volume size in unit of 4KS */
				for (i = 0, pau = 1; cst[i] && cst[i] <= n; i++, pau <<= 1) ;	/* Get from table */
			}
			n_clst = sz_vol / pau;
			if (n_clst > MAX_FAT12) {
				n = n_clst * 2 + 4;		/* FAT size [byte] */
			} else {
				fmt = FS_FAT12;
				n = (n_clst * 3 + 1) / 2 + 3;	/* FAT size [byte] */
			}
			sz_fat = (n + ss - 1) / ss;		/* FAT size [sector] */
			sz_rsv = 1;						/* Number of reserved sectors */
			sz_dir = (DWORD)n_rootdir * SZDIRE / ss;	/* Rootdir size [sector] */
		}
		b_fat = b_vol + sz_rsv;						/* FAT base */
		b_data = b_fat + sz_fat * n_fats + sz_dir;	/* Data base */

		/* Align data base to erase block boundary (for flash memory media) */
		n = ((b_data + sz_blk - 1) & ~(sz_blk - 1)) - b_data;	/* Next nearest erase block from current data base */
		if (fmt == FS_FAT32) {		/* FAT32: Move FAT base */
			sz_rsv += n; b_fat += n;
		} else {					/* FAT: Expand FAT size */
			sz_fat += n / n_fats;
		}

		/* Determine number of clusters and final check of validity of the FAT sub-type */
		if (sz_vol < b_data + pau * 16 - b_vol) LEAVE_MKFS(FR_MKFS_ABORTED);	/* Too small volume */
		n_clst = (sz_vol - sz_rsv - sz_fat * n_fats - sz_dir) / pau;
		if (fmt == FS_FAT32) {
			if (n_clst <= MAX_FAT16) {	/* Too few clusters for FAT32 */
				if (au == 0 && (au = pau / 2) != 0) continue;	/* Adjust cluster size and retry */
				LEAVE_MKFS(FR_MKFS_ABORTED);
			}
		}
		if (fmt == FS_FAT16) {
			if (n_clst > MAX_FAT16) {	/* Too many clusters for FAT16 */
				if (au == 0 && (pau * 2) <= 64) {
					au = pau * 2; continue;		/* Adjust cluster size and retry */
				}
				if ((opt & FM_FAT32)) {
					fmt = FS_FAT32; continue;	/* Switch type to FAT32 and retry */
				}
				if (au == 0 && (au = pau * 2) <= 128) continue;	/* Adjust cluster size and retry */
				LEAVE_MKFS(FR_MKFS_ABORTED);
			}
			if  (n_clst <= MAX_FAT12) {	/* Too few clusters for FAT16 */
				if (au == 0 && (au = pau * 2) <= 128) continue;	/* Adjust cluster size and retry */
				LEAVE_MKFS(FR_MKFS_ABORTED);
			}
		}
		if (fmt == FS_FAT12 && n_clst > MAX_FAT12) LEAVE_MKFS(FR_MKFS_ABORTED);	/* Too many clusters for FAT12 */

		/* Ok, it is the valid cluster configuration */
		break;
	} while (1);

	/* Create FAT VBR */
	memset(buf, 0, ss);
	memcpy(buf + BS_JmpBoot, "\xEB\xFE\x90" "MSDOS5.0", 11);/* Boot jump code (x86), OEM name */
	st_word(buf + BPB_BytsPerSec, ss);				/* Sector size [byte] */
	buf[BPB_SecPerClus] = (BYTE)pau;				/* Cluster size [sector] */
	st_word(buf + BPB_RsvdSecCnt, (WORD)sz_rsv);	/* Size of reserved area */
	buf[BPB_NumFATs] = (BYTE)n_fats;				/* Number of FATs */
	st_word(buf + BPB_RootEntCnt, (WORD)((fmt == FS_FAT32) ? 0 : n_rootdir));	/* Number of root directory entries */
	if (sz_vol < 0x10000) {
		st_word(buf + BPB_TotSec16, (WORD)sz_vol);	/* Volume size in 16-bit LBA */
	} else {
		st_dword(buf + BPB_TotSec32, sz_vol);		/* Volume size in 32-bit LBA */
	}
	buf[BPB_Media] = 0xF8;							/* Media descriptor byte */
	st_word(buf + BPB_SecPerTrk, 63);				/* Number of sectors per track (for int13) */
	st_word(buf + BPB_NumHeads, 255);				/* Number of heads (for int13) */
	st_dword(buf + BPB_HiddSec, b_vol);				/* Volume offset in the physical drive [sector] */
	if (fmt == FS_FAT32) {
		st_dword(buf + BS_VolID32, GET_FATTIME());	/* VSN */
		st_dword(buf + BPB_FATSz32, sz_fat);		/* FAT size [sector] */
		st_dword(buf + BPB_RootClus32, 2);			/* Root directory cluster # (2) */
		st_word(buf + BPB_FSInfo32, 1);				/* Offset of FSINFO sector (VBR + 1) */
		st_word(buf + BPB_BkBootSec32, 6);			/* Offset of backup VBR (VBR + 6) */
		buf[BS_DrvNum32] = 0x80;					/* Drive number (for int13) */
		buf[BS_BootSig32] = 0x29;					/* Extended boot signature */
		memcpy(buf + BS_VolLab32, "NO NAME    " "FAT32   ", 19);	/* Volume label, FAT signature */
	} else {
		st_dword(buf + BS_VolID, GET_FATTIME());	/* VSN */
		st_word(buf + BPB_FATSz16, (WORD)sz_fat);	/* FAT size [sector] */
		buf[BS_DrvNum] = 0x80;						/* Drive number (for int13) */
		buf[BS_BootSig] = 0x29;						/* Extended boot signature */
		memcpy(buf + BS_VolLab, "NO NAME    " "FAT     ", 19);	/* Volume label, FAT signature */
	}
	st_word(buf + BS_55AA, 0xAA55);					/* Signature (offset is fixed here regardless of sector size) */
	if (!FatVolume.disk->write(buf, b_vol, 1)) LEAVE_MKFS(FR_DISK_ERR);	/* Write it to the VBR sector */

	/* Create FSINFO record if needed */
	if (fmt == FS_FAT32) {
		FatVolume.disk->write(buf, b_vol + 6, 1);		/* Write backup VBR (VBR + 6) */
		memset(buf, 0, ss);
		st_dword(buf + FSI_LeadSig, 0x41615252);
		st_dword(buf + FSI_StrucSig, 0x61417272);
		st_dword(buf + FSI_Free_Count, n_clst - 1);	/* Number of free clusters */
		st_dword(buf + FSI_Nxt_Free, 2);			/* Last allocated cluster# */
		st_word(buf + BS_55AA, 0xAA55);
		FatVolume.disk->write(buf, b_vol + 7, 1);		/* Write backup FSINFO (VBR + 7) */
		FatVolume.disk->write(buf, b_vol + 1, 1);		/* Write original FSINFO (VBR + 1) */
	}

	/* Initialize FAT area */
	memset(buf, 0, (UINT)szb_buf);
	sect = b_fat;		/* FAT start sector */
	for (i = 0; i < n_fats; i++) {			/* Initialize FATs each */
		if (fmt == FS_FAT32) {
			st_dword(buf + 0, 0xFFFFFFF8);	/* Entry 0 */
			st_dword(buf + 4, 0xFFFFFFFF);	/* Entry 1 */
			st_dword(buf + 8, 0x0FFFFFFF);	/* Entry 2 (root directory) */
		} else {
			st_dword(buf + 0, (fmt == FS_FAT12) ? 0xFFFFF8 : 0xFFFFFFF8);	/* Entry 0 and 1 */
		}
		nsect = sz_fat;		/* Number of FAT sectors */
		do {	/* Fill FAT sectors */
			n = (nsect > sz_buf) ? sz_buf : nsect;
			if (!FatVolume.disk->write(buf, sect, (UINT) n)) LEAVE_MKFS(FR_DISK_ERR);
			memset(buf, 0, ss);
			sect += n; nsect -= n;
		} while (nsect);
	}

	/* Initialize root directory (fill with zero) */
	nsect = (fmt == FS_FAT32) ? pau : sz_dir;	/* Number of root directory sectors */
	do {
		n = (nsect > sz_buf) ? sz_buf : nsect;
		if (!FatVolume.disk->write(buf, sect, (UINT) n)) LEAVE_MKFS(FR_DISK_ERR);
		sect += n; nsect -= n;
	} while (nsect);

	LEAVE_MKFS(FR_OK);
}
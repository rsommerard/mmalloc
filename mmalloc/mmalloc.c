#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "mmalloc.h"

#define MCHUNK 100

struct mblock_s {
	size_t mb_size;
	struct mblock_s* mb_next;
};

typedef struct mblock_s mblock_t;

/* premier block mémoire libre */
static mblock_t *ffmb = NULL;

void *mmalloc(size_t size) {
	/* précédent block memoire libre */
	mblock_t *pfmb = NULL;

	/* block memoire libre */
	mblock_t *fmb;

	/* Allocation d'un grand espace mémoire
	 * Si on a jamais fait d'appel on fait sbrk
	 */
	if(!ffmb) {
		ffmb = sbrk(sizeof(mblock_t) + MCHUNK);

		if(!ffmb || (size + sizeof(mblock_t) > MCHUNK))
			return NULL;

		ffmb->mb_size = MCHUNK;
		ffmb->mb_next = NULL;
	}

	/* allignement */
	size += size % 4;

	/*cherche un block libre */
	fmb = ffmb;

	while(fmb && fmb->mb_size < size) {
		pfmb = fmb;
		fmb = fmb->mb_next;
	}

	if(!fmb) {
		fmb = sbrk(sizeof(mblock_t) + MCHUNK);

		if(!fmb || (size + sizeof(mblock_t) > MCHUNK))
			return NULL;

        fmb->mb_size = MCHUNK;

        /* garde les adresses croissantes pour le free */
        if(fmb > ffmb) {
            fmb->mb_next = NULL;
            pfmb->mb_next = fmb;
        }
        else {
            fmb->mb_next = ffmb;
            ffmb = fmb;
        }
	}

    mblock_t *nfmb = NULL;
    if(!fmb->mb_next)
        nfmb = (mblock_t *)(((char *)fmb) + sizeof(mblock_t) + size);
    else
        nfmb = fmb->mb_next;

    /* subterfuge lors de la realocation d'un ancien espace memoire */
    if((size + sizeof(mblock_t)) > fmb->mb_size)
        fmb->mb_size = 0;
    else
        nfmb->mb_size = fmb->mb_size - size - sizeof(mblock_t);

    nfmb->mb_next = fmb->mb_next;

    if(!pfmb)
        ffmb = nfmb;
    else
        pfmb->mb_next = nfmb;

    if(ffmb->mb_next == ffmb)
        ffmb->mb_next = NULL;

    fmb->mb_size = size;
    fmb->mb_next = ((mblock_t*)0xDEADBEAF);

    return (mblock_t *)(((char *)fmb) + sizeof(mblock_t));
}

void mfree(void *ptr) {
	mblock_t *fptr;
	mblock_t *pfptr = NULL;
	mblock_t *ptr_mblock = (mblock_t *)(((char *)ptr) - sizeof(mblock_t));

	if(ptr_mblock->mb_next != ((mblock_t*)0xDEADBEAF))
		return;

	fptr = ffmb;
	while(fptr && fptr < ptr_mblock) {
        pfptr = fptr;
		fptr = fptr->mb_next;
	}

	/* ajout du block dans les libres */
	ptr_mblock->mb_next = fptr;
    if(pfptr) {
        pfptr->mb_next = ptr;

        /* fusion
	 * placé après un bloc libre: fusion à gauche */
        if((pfptr + sizeof(mblock_t) + pfptr->mb_size) == ptr_mblock) {
            pfptr->mb_next = fptr;
            pfptr->mb_size += ptr_mblock->mb_size + sizeof(mblock_t);
	    /* fusion
	     * placé avant un bloc libre: fusion à droite */
            if((pfptr + sizeof(mblock_t) + pfptr->mb_size) == fptr) {
                pfptr->mb_size += sizeof(mblock_t) + fptr->mb_size;
                pfptr->mb_next = fptr->mb_next;
            }
        }
        else if((((char*)ptr_mblock) + sizeof(mblock_t) + ptr_mblock->mb_size) == ((char*)fptr)) {
	    /* fusion
	     * placé avant un bloc libre: fusion à droite */
            ptr_mblock->mb_size += sizeof(mblock_t) + fptr->mb_size;
            ptr_mblock->mb_next = fptr->mb_next;
        }
    }
    else if(fptr) {
        ffmb = ptr_mblock;
        ptr_mblock->mb_next = fptr;
	/* placé avant le premier bloc libre: fusion à droite */
        if((((char*)ptr_mblock) + sizeof(mblock_t) + ptr_mblock->mb_size) == ((char*)fptr)) {
            ptr_mblock->mb_size += sizeof(mblock_t) + fptr->mb_size;
            ptr_mblock->mb_next = fptr->mb_next;
        }
    }
    else { /* pas d'autre blocs libre */
        ffmb = ptr_mblock;
        ptr_mblock->mb_next = NULL;
    }
}

void *mcalloc(unsigned int nb_elem, size_t size) {
	char *tab_alloc = ((char *)mmalloc(nb_elem * size));

	if(tab_alloc)
		memset(tab_alloc,0,nb_elem * size);

	return tab_alloc;
}

void *mrealloc(void *ptr, size_t size) {
	char *new_alloc = ((char *)mmalloc(size));

	if(new_alloc) {
		char *tmp_alloc = ((char *)ptr);
		memcpy(new_alloc,tmp_alloc,size);
		mfree(ptr);
		return ((char *)new_alloc);
	}
	
	return NULL;
}

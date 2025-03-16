/**
 * @file fsm.c
 * @brief Library to create Finite State Machines using composition.
 *
 * This library is expected to be used using composition
 * @author Teachers from the Departamento de Ingeniería Electrónica. Original authors: José M. Moya and Pedro J. Malagón. Latest contributor: Román Cárdenas.
 * @date 2023-09-20
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>

//GCOVR_EXCL_START
void* __attribute__((weak)) fsm_malloc(size_t s) 
{
    return malloc(s);
}

void __attribute__((weak)) fsm_free(void* p)
{
    free(p);
} 
//GCOVR_EXCL_STOP

/* Other includes */
#include "fsm.h"

static bool fsm_check_transitions(fsm_trans_t *p_tt)
{
    if (p_tt == NULL)
    {
        return false;
    }
    if ((p_tt->orig_state == -1) || (p_tt->dest_state == -1))
    {
        return false;
    }
    return true;
}

static void fsm_init_no_check(fsm_t *p_fsm, fsm_trans_t *p_tt)
{
    p_fsm->p_tt = p_tt;
    p_fsm->current_state = p_tt->orig_state;
}

fsm_t *fsm_new(fsm_trans_t *p_tt)
{
    if (!fsm_check_transitions(p_tt)) {
        return NULL;
    }
    fsm_t *p_fsm = (fsm_t *)fsm_malloc(sizeof(fsm_t));
    if (p_fsm != NULL)
    {
        fsm_init_no_check(p_fsm, p_tt);
    }
    return p_fsm;
}

void fsm_destroy(fsm_t *p_fsm)
{
    if (p_fsm != NULL){
        fsm_free(p_fsm);
    }
}

int fsm_init(fsm_t *p_fsm, fsm_trans_t *p_tt)
{
    //int max_tran_num = 0;
    int number_states = 0;

    /**while(p_tt[max_tran_num].orig_state != -1) {
        max_tran_num++;
    }*/ // No hay que hacer conteo

    if (p_fsm == NULL) {
        return 0;
    }
    if (!fsm_check_transitions(p_tt)) {
        return 0;
    }
    fsm_init_no_check(p_fsm, p_tt);
    //return 1;


    for(int i = 0; i <= FSM_MAX_TRANSITIONS + 1; i++){
        bool check = fsm_check_transitions(&p_tt[i]);
        if (number_states <= FSM_MAX_TRANSITIONS && check){
            number_states++;
        } else if (number_states > FSM_MAX_TRANSITIONS){
            number_states = 0;
        } else {
            return number_states;
        }
    }
    return number_states;
    
}

int fsm_get_state(fsm_t *p_fsm)
{
    return p_fsm->current_state;
}

void fsm_set_state(fsm_t *p_fsm, int state)
{
    p_fsm->current_state = state;
}

int fsm_fire(fsm_t *p_fsm){

    fsm_trans_t *p_t;
    bool b_fsmFire = true;

    for (p_t = p_fsm->p_tt; p_t->orig_state >= 0; ++p_t){
        if ((p_fsm->current_state == p_t->orig_state)){
            if (p_t->in(p_fsm)){
                p_fsm->current_state = p_t->dest_state;
                if (p_t->out){
                    p_t->out(p_fsm);
                }  
                return 1;
            }else{
                b_fsmFire = false;
            }
        }
    }

    if(b_fsmFire){
        return -1;
    }else{
        return 0;
    }
    
}


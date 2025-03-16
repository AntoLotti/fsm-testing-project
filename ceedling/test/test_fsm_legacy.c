#include "unity.h"

#define TEST_CASE(...)
#define TEST_RANGE(...)

#include "fsm.h"
#include "mock_test_fsm.h"

#include <stdlib.h>

/** Comandos testeo
 * ceedling clobber
 * ceedling gcov:all
 * ceedling test:all
 * ceedling gcov:all utils:gcov*/ 

/**
 * @file test_fsm_legacy.c
 * @author Alvaro Montesano
 * @author Antonio Lotti
 * @brief Tests for existing fsm module
 * @version 0.1
 * @date 2024-04-09
 * 
 */

/**
 * @brief Stub or Callback for fsm_malloc that calls real malloc. 
 * 
 * @param[in] s Amount of bytes to allocate
 * @param[in] n Amount of calls to this function
 * 
 * @return pointer to allocated memory if success; NULL if fails
 * 
 */
static void* cb_malloc(size_t s, int n){
    return malloc(s);
}

/**
 * @brief Stub or Callback for fsm_free that calls real free. 
 * 
 * @param[in] p Pointer to allocated memory to free
 * @param[in] n Amount of calls to this function
 * 
 */
static void cb_free(void* p, int n){
    return free(p);
}

/**
 * @brief Function that returns a matrix wtih a number of valid states that maches the number given
 * 
 * @param[in] p_rr Fsm_trans_t array that contains the states
 * @param[in] n Int that indicates the number of the valid states
 * @param[in] m Int that indicates the number of the invalid states
 * 
 */
static void validTransitionTable (fsm_trans_t* p_rr, int n, int m){ 

    for(int i = 0; i < n; i++){
        p_rr[i] = (fsm_trans_t) {i, NULL, i+1, NULL};
    }

    for(int j = n; j < n + m; j++){
        p_rr[j] = (fsm_trans_t) {-1, NULL, -1, NULL};
    }

    p_rr [n + m] = (fsm_trans_t) {-1, NULL, -1, NULL};
    return;

}

void setUp(void){
}

void tearDown(void){
}

/**
 * @brief Comprueba que la funcion de fsm_new devuelve NULL 
 *        y no llama a fsm_malloc si la tabla de transiciones es NULL 
 */
void test_fsm_new_nullWhenNullTransition(void){

    fsm_t *f = (fsm_t*)1;
    f = fsm_new(NULL);
    // No usado en este test
    /**fsm_malloc_AddCallback(cb_malloc); // AddCallback con una sola llamada es suficiente para varias llamadas a malloc.
    fsm_malloc_ExpectAnyArgsAndReturn(NULL); // ExpectedReturn devuelve lo que deseamos que devuelva la función en cada momento. (Tantas ExpectedReturn como llamadas a malloc)*/
    TEST_ASSERT_EQUAL (NULL, f);

}

/**
 * @brief Comprueba que la función de inicialización devuelve false si el puntero a la maquina de estado es NULL 
 *
 */
void test_fsm_init_falseWhenNullFsm(void){

    fsm_trans_t tt[] = {
        {0, is_true, 1, do_nothing},
        {-1, NULL, -1, NULL}
    };
    fsm_t *f = NULL;
    int f_bool = fsm_init(f, tt);
    TEST_ASSERT_EQUAL(0, f_bool);

}

/**
 * @brief Función de inicializacion devuelve false si la tabla de transiciones es nula
 * 
 */
void test_fsm_init_falseWhenNullTransitions(void){

    fsm_trans_t tt[] = {{-1, NULL, -1, NULL}};
    fsm_t *f = (fsm_t*)1;
    int f_bool = fsm_init(f, tt);
    TEST_ASSERT_EQUAL(0, f_bool);

}

/**
* @brief La máquina de estados devuelve NULL 
*        y no llama a fsm_malloc si el estado de origen 
*        de la primera transición es -1 (fin de la tabla)
*/
void test_fsm_nullWhenFirstOrigStateIsMinusOne (void){

  fsm_trans_t tt[] = {{-1, is_true, 1, do_nothing}};
  fsm_t *f = (fsm_t*)1;
  f = fsm_new(tt);
  TEST_ASSERT_EQUAL (NULL, f);

}

/**
 * @brief La máquina de estados devuelve NULL y no llama a fsm_malloc si el estado de destino de la primera transición es -1 (fin de la tabla)
 * 
 */
void test_fsm_nullWhenFirstDstStateIsMinusOne (void) {
  
  fsm_trans_t tt[] = {{1, is_true, -1, do_nothing}};
  fsm_t *f = (fsm_t*)1;
  f = fsm_new(tt);
  TEST_ASSERT_EQUAL (NULL, f);

}

/**
 * @brief La máquina de estados devuelve NULL y no llama a fsm_malloc si la función de comprobación de la primera transición es NULL (fin de la tabla)
 * 
 */
void test_fsm_nullWhenFirstCheckFunctionIsNull (void) {
  
  fsm_trans_t tt[] = {{-1, NULL, -1, NULL}};
  fsm_t *f = (fsm_t*)1;
  f = fsm_new(tt);
  TEST_ASSERT_EQUAL (NULL, f);

}

/**
 * @brief Devuelve puntero no NULL y llama a fsm_malloc (Callback) al crear la maquina de estados con una transición válida con función de actualización (salida) NULL o no NULL.
 *        Hay que liberar la memoria al final llamando a free
 * 
 */
TEST_CASE(NULL)
TEST_CASE(do_nothing)
void test_fsm_new_nonNullWhenOneValidTransitionCondition(fsm_output_func_t out){

    fsm_trans_t tt[] = {
        {0, is_true, 1, out},
        {-1, NULL, -1, NULL}
    };
    fsm_malloc_AddCallback(cb_malloc);
    fsm_malloc_ExpectAnyArgsAndReturn(0);
    is_true_ExpectAnyArgsAndReturn(1);
    do_nothing_Ignore();
    fsm_t *f = (fsm_t*)1;
    f = fsm_new (tt);
    fsm_fire(f);
    TEST_ASSERT_NOT_EQUAL(NULL, f);
    free(f);

}


/**
 * @brief Estado inicial corresponde al estado de entrada de la primera transición de la lista al crear una maquina de estados y es valido. 
 * 
 */
void test_fsm_new_fsmGetStateReturnsOrigStateOfFirstTransitionAfterInit(void){

    fsm_trans_t tt[] = {
        {0, is_true, 1, do_nothing},
        {-1, NULL, -1, NULL}
    };
    fsm_malloc_AddCallback(cb_malloc);
    fsm_malloc_ExpectAnyArgsAndReturn(0);
    fsm_t* f = (fsm_t*)1;
    f = fsm_new(tt);
    TEST_ASSERT_EQUAL(tt[0].orig_state, fsm_get_state(f));
    free(f);

}

/**
 * @brief La maquina de estado no transiciona si la funcion devuelve 0
 * 
 */
void test_fsm_fire_isTrueReturnsFalseMeansDoNothingIsNotCalledAndStateKeepsTheSame(void){

    fsm_trans_t tt[] = {
        {0, is_true, 1, do_nothing},
        {-1, NULL, -1, NULL}
    };
    is_true_ExpectAnyArgsAndReturn(0);
    fsm_malloc_Stub(cb_malloc); // Realiza la llamada a la función, en este caso cb_malloc, y realiza un ignore. No realiza un expected.
    //fsm_malloc_AddCallback(cb_malloc);
    //fsm_malloc_IgnoreAndReturn(0);
    fsm_t* f = (fsm_t*)1;    
    f = fsm_new(tt);
    fsm_fire(f);
    int res = fsm_get_state(f);
    TEST_ASSERT_NOT_EQUAL (1, res);
    free(f);

}

/**
 * @brief Comprueba que el puntero pasado a fsm_fire es pasado a la función de guarda cuando se comprueba una transición
 * 
 */
void test_fsm_fire_checkFunctionCalledWithFsmPointerFromFsmFire(void){
    
    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},
        {-1, NULL, -1, NULL}
    };
    fsm_malloc_Stub(cb_malloc);
    fsm_t *f = fsm_new(tt);

    is_true_ExpectAndReturn(f, true);
    fsm_fire(f);
    
}

/** 
 * @brief Comprueba que el fsm_fire funciona y tiene el estado correcto cuando la transición devuelve true (cambia) y cuando devuelve false (mantiene)
 * 
 */
TEST_CASE(false, 0)
TEST_CASE(true, 1)
void test_fsm_fire_checkFunctionIsCalledAndResultIsImportantForTransition(bool returnValue, int expectedState){

    fsm_trans_t tt[] = {
        {0, is_true, 1, do_nothing},
        {-1, NULL, -1, NULL}
    };
    is_true_ExpectAnyArgsAndReturn(returnValue);
    do_nothing_Ignore();
    fsm_malloc_Stub(cb_malloc);
    fsm_t* f = (fsm_t*)1;
    f = fsm_new(tt);
    fsm_trans_t ff = tt[0];
    fsm_fire(f);
    TEST_ASSERT_EQUAL(expectedState, f->current_state);
    
    /**is_true_ExpectAnyArgsAndReturn(1);
    do_nothing_ExpectAnyArgs();
    fsm_t f;
    fsm_init(&f, tt);
    fsm_trans_t ff = tt[0];
    int org_State = ff.orig_state;
    int dst_State = ff.dest_state;
    TEST_ASSERT_EQUAL (org_State, f.current_state);
    fsm_fire(&f);
    TEST_ASSERT_EQUAL (dst_State, f.current_state);*/

    // Hay que hacerlo de la manera en la que lo piden

}


/**
 * @brief La creación de una máquina de estados devuelve NULL si la reserva de memoria falla (Mock, no Stub)
 * 
 */
void test_fsm_new_nullWhenFsmMallocReturnsNull(void){

    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},
        {1, is_true, 0, NULL},
        {-1, NULL, -1, NULL}
    };
    fsm_malloc_AddCallback(NULL);
    fsm_malloc_ExpectAnyArgsAndReturn(0);
    fsm_t* f = (fsm_t*)1;
    f = fsm_new(tt);
    TEST_ASSERT_EQUAL(NULL, f);

}

/**
 * @brief Llamar a fsm_destroy provoca una llamada a fsm_free (Mock, no Stub)
 * 
 */
void test_fsm_destroy_callsFsmFree(void){
    
    fsm_free_ExpectAnyArgs(); // Doble usado para monitorización, no necesario CallBack para devolver valor
    fsm_t* f = (fsm_t*)1;
    fsm_destroy(f);
    // Cuando se libera un puntero no se puede comprobar que se ha liberado. Simplemente comprobamos que se ha llamado a la función free.

}

/**
 * @brief Comprueba que solo se llame a la función de guarda que toca según el estado actual
 * 
 */
void test_fsm_fire_callsFirstIsTrueFromState0AndThenIsTrue2FromState1(void){

    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},
        {1, is_true2, 0, NULL},   //Descomentar cuando se haya declarado una nueva función para mock is_true2
        {-1, NULL, -1, NULL}
    };

    fsm_t *f = (fsm_t*)1;
    int res1;
    int res2;

    fsm_malloc_Stub(cb_malloc);
    is_true_ExpectAnyArgsAndReturn(1);
    is_true2_ExpectAnyArgsAndReturn(1);

    //fsm_init(&f, tt);
    f = fsm_new(tt);

    fsm_fire(f);
    res1 = fsm_get_state(f);
    TEST_ASSERT_EQUAL(1, res1);

    fsm_fire(f);
    res2 = fsm_get_state(f);
    TEST_ASSERT_EQUAL(0, res2);

}

/**
 * @brief Comprueba que se pueden crear dos instancias de máquinas de estados simultánteas y son punteros distintos
 * 
 */
void test_fsm_new_calledTwiceWithSameValidDataCreatesDifferentInstancePointer(void){

    fsm_trans_t tt[] = {
        {0, is_true, 1, NULL},
        {-1, NULL, -1, NULL}
    };
    fsm_malloc_AddCallback(cb_malloc);
    fsm_malloc_ExpectAnyArgsAndReturn(0);
    fsm_malloc_ExpectAnyArgsAndReturn(0);
    fsm_t* fsm1 = (fsm_t*)1;
    fsm_t* fsm2 = (fsm_t*)1;
    fsm1 = fsm_new(tt);
    fsm2 = fsm_new(tt);

    TEST_ASSERT_NOT_EQUAL(fsm1, fsm2);

}



// 23_04_24 NewTests


/**
 * @brief 1.- fsm_init devuelve int con el número de transiciones
 *        válidas (con máximo de 128 indicado en un #define
 *        FSM_MAX_TRANSITIONS). Si es mayor, devuelve 0.
 */
void test_fsm_initReturnStatesNumerIfFSMTransitionsShorterThan128(void){ //16

    int valid_states = 16;
    int invalid_states = 0;
    int states = valid_states + invalid_states + 1;

    fsm_trans_t tt[states];
    validTransitionTable(tt, valid_states, invalid_states);
    fsm_t f;
    int t = fsm_init(&f, tt);   
    TEST_ASSERT_EQUAL(16, t);

}
void test_fsm_initReturnStatesNumerIfFSMTransitionsShorterThan128AndInvalidTransitions(void){ //16

    int valid_states = 16;
    int invalid_states = 5;
    int states = valid_states + invalid_states + 1;

    fsm_trans_t tt[states];
    validTransitionTable(tt, valid_states, invalid_states);
    fsm_t f;
    int t = fsm_init(&f, tt);   
    TEST_ASSERT_EQUAL(16, t);

}
void test_fsm_initReturnStatesNumerIfFSMTransitionsIs128(void){ //128

    int valid_states = 128;
    int invalid_states = 0;
    int states = valid_states + invalid_states + 1;

    fsm_trans_t tt[states];
    validTransitionTable(tt, valid_states, invalid_states);
    fsm_t f;
    int t = fsm_init(&f, tt);   
    TEST_ASSERT_EQUAL(128, t);

}
void test_fsm_initReturnStatesNumerIfFSMTransitionsBiggerThan128(void){ //130

    int valid_states = 130;
    int invalid_states = 0;
    int states = valid_states + invalid_states + 1;

    fsm_trans_t tt[states];
    validTransitionTable(tt, valid_states, invalid_states);
    fsm_t f;
    int t = fsm_init(&f, tt);   
    TEST_ASSERT_EQUAL(0, t);

}
void test_fsm_initReturnStatesNumerIfFSMTransitionsNoValidTransition(void){ //0

    int valid_states = 0;
    int invalid_states = 0;
    int states = valid_states + invalid_states + 1;

    fsm_trans_t tt[states];
    validTransitionTable(tt, valid_states, invalid_states);
    fsm_t f;
    int t = fsm_init(&f, tt);   
    TEST_ASSERT_EQUAL(0, t);

}

/**
 * @brief 2.- Una función de guarda NULL en una transición equivalente
 *        a que se cumple siempre (equivale a devuelve true).
 */
void test_fsm_checkNULLTransitionAllwaysTrue(void){

    fsm_trans_t tt [] = {
        {0, NULL, 1, NULL},
        {-1, NULL, -1, NULL}
    };
    fsm_malloc_Stub(cb_malloc);
    fsm_t *f = (fsm_t*)1;
    f = fsm_new(tt);
    TEST_ASSERT_NOT_EQUAL(NULL, f);

}

/**
 * @brief 3.- Añadir valor de retorno int a fsm_fire:
 *        -1: si no hay ninguna transición para el estado actual.
 *         0: si hay transiciones para el estado actual pero la función de
 *            guarda devuelve false.
 *         1: si hay al menos una transición para el estado actual con la
 *            función de guarda true.
 */
void test_fsm_notTransitionForActualState(void){

    fsm_trans_t tt [] = {
        {0, NULL, 1 , NULL},
        {-1, NULL, -1, NULL}
    };

    fsm_malloc_Stub(cb_malloc);

    int v_fsmFire;
    fsm_t *f = (fsm_t*)1;

    f = fsm_new(tt);
    fsm_set_state(f, 1);
    v_fsmFire = fsm_fire(f);

    TEST_ASSERT_EQUAL(-1, v_fsmFire);

}

void test_fsm_TransitionForTheActualStateButFalseTransition(void){

    fsm_trans_t tt [] = {
        {0, is_true, 1, NULL},
        {-1, NULL, -1, NULL}
    };
    fsm_malloc_Stub(cb_malloc);
    is_true_ExpectAnyArgsAndReturn(false);

    int v_fsmFire;
    fsm_t *f = (fsm_t*)1;

    f = fsm_new(tt);
    v_fsmFire = fsm_fire(f);

    TEST_ASSERT_EQUAL(0, v_fsmFire);

}

void test_fsm_ThereIsAlmostOneTransitionForActualState(void){

    fsm_trans_t tt [] = {
        {0, is_true, 1, NULL},
        {-1, NULL, -1, NULL}
    };
    fsm_malloc_Stub(cb_malloc);
    is_true_ExpectAnyArgsAndReturn(true);

    int v_fsmFire;
    fsm_t *f = (fsm_t*)1;

    f = fsm_new(tt);
    v_fsmFire = fsm_fire(f);

    TEST_ASSERT_EQUAL(1, v_fsmFire);

}


/**
 * @brief 4.- Llamada a fsm_destroy llama a fsm_free solo si el
 *        puntero pasado no es NULL.
 */
void test_fsm_destroy_dontdoFreeIfPointerNULL(void){

    fsm_t* f = NULL;
    fsm_destroy(f);
    
}
void test_fsm_destroy_doFreeIfPointerNotNULL(void){ 

    fsm_trans_t tt [] = {
        {0, NULL, 1, NULL},
        {-1, NULL, -1, NULL}
    };
    fsm_malloc_AddCallback(cb_malloc);
    fsm_malloc_ExpectAnyArgsAndReturn(0);
    fsm_free_AddCallback(cb_free);
    fsm_free_ExpectAnyArgs();
    fsm_t* f = fsm_new(tt);
    fsm_destroy(f);
    
}
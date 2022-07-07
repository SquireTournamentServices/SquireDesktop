#pragma once

#define OK 0

// Enums for all of the ret vals in ./squire_core/src/ff.rs
typedef enum ffi_from_preset_t {
    FROM_PRESET_OK=0,
    FROM_PRESET_NAME_ERROR=1,
    FROM_PRESET_FORMAT_ERROR=2
} ffi_from_preset_t;

#define is_from_preset_err(ret) (ret != OK)

typedef enum ffi_apply_op_t {
    APPLY_OP_OK=0,
    APPLY_OP_STATUS_ERROR=1,
    APPLY_OP_PLAYER_LOOKUP_ERROR=2,
    APPLY_OP_ROUND_LOOKUP_ERROR=3,
    APPLY_OP_DECK_LOOKUP_ERROR=4,
    APPLY_OP_REG_CLOSED_ERROR=5,
    APPLY_OP_PLAYER_NOT_IN_ROUND_ERROR=6,
    APPLY_OP_NO_ACTIVE_ROUND_ERROR=7,
    APPLY_OP_INVALID_BYE_ERROR=8,
    APPLY_OP_ACTIVE_MATCHES_ERROR=9,
    APPLY_OP_NOT_CHECKED_IN_ERROR=10,
    APPLY_OP_PAIRING_SYSTEM_ERROR=11,
    APPLY_OP_SCORING_SYSTEM_ERROR=12
} ffi_apply_op_t;

#define is_apply_op_err(ret) (ret != OK)

typedef enum ffi_get_player_t {
    GET_PLAYER_OK=0,
    GET_PLAYER_ERR=1
} ffi_get_player_t;

typedef enum ffi_get_round_t {
    GET_ROUND_OK=0,
    GET_ROUND_ERR=1
} ffi_get_round_t;

typedef enum ffi_get_player_deck_t {
    GET_PLAYER_DECK_OK=0,
    GET_PLAYER_DECK_CONV_ERR=1,
    GET_PLAYER_DECK_PLAYER_LOOKUP_ERROR=2,
    GET_PLAYER_DECK_DECK_LOOKUP_ERROR=3
} ffi_get_player_deck_t;

typedef enum ffi_new_registered_player_op_t {
    NEW_REGISTERED_PLAYER_OP_OK=0,
    NEW_REGISTERED_PLAYER_OP_ERR=1
} ffi_new_registered_player_op_t;

typedef enum ffi_new_remove_deck_op_t {
    NEW_REMOVE_DECK_OP_OK=0,
    NEW_REMOVE_DECK_OP_ERR=1
} ffi_new_remove_deck_op_t;


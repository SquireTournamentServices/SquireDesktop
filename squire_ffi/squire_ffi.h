/* AUTO GENERATED SquireFFI BINDINGS DO NOT MODIFY BY HAND. */

#pragma once

/* Generated with cbindgen:0.24.3 */

/* Warning: this file is autogenerated by cbindgen. Don't modify this manually. */

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>
#include <cassert>
#define SQUIRE_FFI

namespace squire_ffi {

/**
 * The number of 100 nanosecond ticks between the RFC4122 epoch
 * (`1582-10-15 00:00:00`) and the Unix epoch (`1970-01-01 00:00:00`).
 */
constexpr static const uint64_t sc_UUID_TICKS_BETWEEN_EPOCHS = 122192928000000000;

/**
 * The registration status of a player
 */
enum class sc_PlayerStatus
{
  /**
   * The player is registered for the tournament
   */
  Registered,
  /**
   * The player has been dropped from the tournament
   */
  Dropped,
};

/**
 * The status of a round has exactly four states. This enum encodes them
 */
enum class sc_RoundStatus
{
  /**
   * The round is still active and nothing has been recorded
   */
  Open,
  /**
   * All results are in and all players have certified the result
   */
  Certified,
  /**
   * The round is no long consider to be part of the tournament, but is not deleted to prevent
   * naming collisions.
   */
  Dead,
};

/**
 * An enum that encode the initial values of a tournament
 */
enum class sc_TournamentPreset
{
  /**
   * The tournament will have a swiss pairing system and a standard scoring system
   */
  Swiss,
  /**
   * The tournament will have a fluid pairing system and a standard scoring system
   */
  Fluid,
};

/**
 * An enum that encodes all the statuses of a tournament
 */
enum class sc_TournamentStatus
{
  /**
   * The tournament can not create rounds
   */
  Planned,
  /**
   * All functionalities are unlocked
   */
  Started,
  /**
   * All functionalities except status changes are locked
   */
  Frozen,
  /**
   * The tournament is over after starting
   */
  Ended,
  /**
   * The tournament is over and was never started
   */
  Cancelled,
};

/**
 * The enum that encodes what could go wrong while performing an action
 */
struct sc_ActionError;

/**
 * The core model for an account for a tournament admin
 */
struct sc_Admin;

/**
 * The core model for an account for a tournament judge
 */
struct sc_Judge;

/**
 * The core player model.
 * This model only contains information about the player and what they have registered. All
 * information about their matches, standing, etc is derived externally.
 *
 * A player has two primary identifiers, their id and name. They have an opional `game_name` for
 * use in digital tournament where their name on a specific platform might be needed.
 */
struct sc_Player;

/**
 * A "round" might also be known as a "match" in some circles. This contains of at least two
 * players playing at least one games against each other; however, a round can also encode a bye,
 * a free win for exactly one player.
 *
 * Each round tracks its start time, expected length, and any extentions. The round clock starts
 * immediately after being created.
 *
 * Results are recorded for each player as well as for each drawn game. After that, the results
 * need to be confirmed by all players or by an admin.
 *
 * After the results have been confirmed, the round is consider certified and a winner is declared
 * (if possible)
 */
struct sc_Round;

/**
 * The core model for an account for a user
 */
struct sc_SquireAccount;

/**
 * The struct that contains everything needed to manage and query the tournament model.
 */
struct sc_SquireRuntime;

/**
 * The core tournament structure
 */
struct sc_Tournament;

/**
 * A 128-bit (16 byte) buffer containing the UUID.
 *
 * # ABI
 *
 * The `Bytes` type is always guaranteed to be have the same ABI as [`Uuid`].
 */
using sc_Bytes = uint8_t[16];

/**
 * A Universally Unique Identifier (UUID).
 *
 * # Examples
 *
 * Parse a UUID given in the simple format and print it as a urn:
 *
 * ```
 * # use uuid::Uuid;
 * # fn main() -> Result<(), uuid::Error> {
 * let my_uuid = Uuid::parse_str("a1a2a3a4b1b2c1c2d1d2d3d4d5d6d7d8")?;
 *
 * println!("{}", my_uuid.urn());
 * # Ok(())
 * # }
 * ```
 *
 * Create a new random (V4) UUID and print it out in hexadecimal form:
 *
 * ```
 * // Note that this requires the `v4` feature enabled in the uuid crate.
 * # use uuid::Uuid;
 * # fn main() {
 * # #[cfg(feature = "v4")] {
 * let my_uuid = Uuid::new_v4();
 *
 * println!("{}", my_uuid);
 * # }
 * # }
 * ```
 *
 * # Formatting
 *
 * A UUID can be formatted in one of a few ways:
 *
 * * [`simple`](#method.simple): `a1a2a3a4b1b2c1c2d1d2d3d4d5d6d7d8`.
 * * [`hyphenated`](#method.hyphenated):
 *   `a1a2a3a4-b1b2-c1c2-d1d2-d3d4d5d6d7d8`.
 * * [`urn`](#method.urn): `urn:uuid:A1A2A3A4-B1B2-C1C2-D1D2-D3D4D5D6D7D8`.
 * * [`braced`](#method.braced): `{a1a2a3a4-b1b2-c1c2-d1d2-d3d4d5d6d7d8}`.
 *
 * The default representation when formatting a UUID with `Display` is
 * hyphenated:
 *
 * ```
 * # use uuid::Uuid;
 * # fn main() -> Result<(), uuid::Error> {
 * let my_uuid = Uuid::parse_str("a1a2a3a4b1b2c1c2d1d2d3d4d5d6d7d8")?;
 *
 * assert_eq!(
 *     "a1a2a3a4-b1b2-c1c2-d1d2-d3d4d5d6d7d8",
 *     my_uuid.to_string(),
 * );
 * # Ok(())
 * # }
 * ```
 *
 * Other formats can be specified using adapter methods on the UUID:
 *
 * ```
 * # use uuid::Uuid;
 * # fn main() -> Result<(), uuid::Error> {
 * let my_uuid = Uuid::parse_str("a1a2a3a4b1b2c1c2d1d2d3d4d5d6d7d8")?;
 *
 * assert_eq!(
 *     "urn:uuid:a1a2a3a4-b1b2-c1c2-d1d2-d3d4d5d6d7d8",
 *     my_uuid.urn().to_string(),
 * );
 * # Ok(())
 * # }
 * ```
 *
 * # Endianness
 *
 * The specification for UUIDs encodes the integer fields that make up the
 * value in big-endian order. This crate assumes integer inputs are already in
 * the correct order by default, regardless of the endianness of the
 * environment. Most methods that accept integers have a `_le` variant (such as
 * `from_fields_le`) that assumes any integer values will need to have their
 * bytes flipped, regardless of the endianness of the environment.
 *
 * Most users won't need to worry about endianness unless they need to operate
 * on individual fields (such as when converting between Microsoft GUIDs). The
 * important things to remember are:
 *
 * - The endianness is in terms of the fields of the UUID, not the environment.
 * - The endianness is assumed to be big-endian when there's no `_le` suffix
 *   somewhere.
 * - Byte-flipping in `_le` methods applies to each integer.
 * - Endianness roundtrips, so if you create a UUID with `from_fields_le`
 *   you'll get the same values back out with `to_fields_le`.
 *
 * # ABI
 *
 * The `Uuid` type is always guaranteed to be have the same ABI as [`Bytes`].
 */
using sc_Uuid = sc_Bytes;

/**
 * A generic type-checked wrapper around a Uuid (to reduce boilerplate and redudent code)
 */
template<typename T>
struct sc_TypeId
{
  sc_Uuid _0;
};

/**
 * A type-checked Uuid for tournaments
 */
using sc_TournamentId = struct __sc_TournamentId{ sc_Uuid _0; };

/**
 * A type-checked Uuid for players
 */
using sc_PlayerId = struct __sc_PlayerId{ sc_Uuid _0; };

/**
 * A type-checked Uuid for rounds
 */
using sc_RoundId = struct __sc_RoundId{ sc_Uuid _0; };

/**
 * A type-checked Uuid for tournament admin
 */
using sc_AdminId = struct __sc_AdminId{ sc_Uuid _0; };

/**
 * A type-checked Uuid for user accounts
 */
using sc_SquireAccountId = struct __sc_SquireAccountId{ sc_Uuid _0; };


/// This is ghastly wrapper for ratio32
typedef struct ratio32 {
    int _0;
    int _1;
} ratio32;

/// Turns a rational32 or ratio32 into a floating point number
float ratio32ToFloat(ratio32 r);

using sc_Rational32 = ratio32;
using sc_r64 = sc_Rational32;

/**
 * The score type used by the standard scoring system
 */
struct sc_StandardScore
{
  /**
   * The number of match points a player has
   */
  sc_r64 match_points;
  /**
   * The number of game points a player has
   */
  sc_r64 game_points;
  /**
   * The match win percentage of a player
   */
  sc_r64 mwp;
  /**
   * The game win percentage of a player
   */
  sc_r64 gwp;
  /**
   * The average match win percentage of a player's opponents
   */
  sc_r64 opp_mwp;
  /**
   * The average game win percentage of a player's opponents
   */
  sc_r64 opp_gwp;
  /**
   * Whether or not match points should be considered
   */
  bool include_match_points;
  /**
   * Whether or not game points should be considered
   */
  bool include_game_points;
  /**
   * Whether or not match win percentage should be considered
   */
  bool include_mwp;
  /**
   * Whether or not game win percentage should be considered
   */
  bool include_gwp;
  /**
   * Whether or not opponent's match win percentage should be considered
   */
  bool include_opp_mwp;
  /**
   * Whether or not opponent's game win percentage should be considered
   */
  bool include_opp_gwp;
};

/**
 * A struct used to pass scores to scores across the language boundary
 */
template<typename S>
struct sc_PlayerScore
{
  sc_PlayerId pid;
  S score;
};

/**
 * A type-checked Uuid for tournament judges
 */
using sc_JudgeId = struct __sc_JudgeId{ sc_Uuid _0; };

extern "C" {

/**
 * Closes a tournament removing it from the internal FFI state
 */
bool close_tourn(sc_TournamentId tid);

/**
 * Closes a tournament removing it from the internal FFI state
 */
bool close_tourn(sc_TournamentId self);

/**
 * Call this in main()
 * Inits the internal structs of squire lib for FFI.
 */
void init_squire_ffi();

/**
 * Call this in main()
 * Inits the internal structs of squire lib for FFI.
 */
void init_squire_ffi();

/**
 * Loads a tournament from a file via serde
 * The tournament is then registered (stored on the heap)
 * CStr path to the tournament (alloc and, free on Cxx side)
 * Returns a NULL UUID (all 0s) if there is an error
 */
sc_TournamentId load_tournament_from_file(const char *__file);

/**
 * Loads a tournament from a file via serde
 * The tournament is then registered (stored on the heap)
 * CStr path to the tournament (alloc and, free on Cxx side)
 * Returns a NULL UUID (all 0s) if there is an error
 */
sc_TournamentId load_tournament_from_file(const char *__file);

/**
 * Creates a tournament from the settings provided
 * Returns a NULL UUID (all 0s) if there is an error
 */
sc_TournamentId new_tournament_from_settings(const char *__file, const char *__name, const char *__format, sc_TournamentPreset preset, bool use_table_number, uint8_t game_size, uint8_t min_deck_count, uint8_t max_deck_count, bool reg_open, bool require_check_in, bool require_deck_reg);

/**
 * Creates a tournament from the settings provided
 * Returns a NULL UUID (all 0s) if there is an error
 */
sc_TournamentId new_tournament_from_settings(const char *__file, const char *__name, const char *__format, sc_TournamentPreset preset, bool use_table_number, uint8_t game_size, uint8_t min_deck_count, uint8_t max_deck_count, bool reg_open, bool require_check_in, bool require_deck_reg);

/**
 * Returns the player's game name if they can be found
 * NULL is returned on error or, failure to find
 */
const char *pid_game_name(sc_PlayerId pid, sc_TournamentId tid);

/**
 * Returns the player's game name if they can be found
 * NULL is returned on error or, failure to find
 */
const char *pid_game_name(sc_PlayerId self, sc_TournamentId tid);

/**
 * Returns the player name if they can be found
 * NULL is returned on error or, failure to find
 */
const char *pid_name(sc_PlayerId pid, sc_TournamentId tid);

/**
 * Returns the player name if they can be found
 * NULL is returned on error or, failure to find
 */
const char *pid_name(sc_PlayerId self, sc_TournamentId tid);

/**
 * Returns a raw pointer to rounds that a player is in
 * This is an array that is terminated by the NULL UUID
 * This is heap allocted, please free it
 * Returns NULL on error
 */
const sc_RoundId *pid_rounds(sc_PlayerId pid, sc_TournamentId tid);

/**
 * Returns a raw pointer to rounds that a player is in
 * This is an array that is terminated by the NULL UUID
 * This is heap allocted, please free it
 * Returns NULL on error
 */
const sc_RoundId *pid_rounds(sc_PlayerId self, sc_TournamentId tid);

/**
 * Returns the player's status if they can be found
 * Dropped on error.
 */
sc_PlayerStatus pid_status(sc_PlayerId pid, sc_TournamentId tid);

/**
 * Returns the player's status if they can be found
 * Dropped on error.
 */
sc_PlayerStatus pid_status(sc_PlayerId self, sc_TournamentId tid);

/**
 * Confirms a player for the match result
 * false on error
 */
bool rid_confirm_player(sc_RoundId rid, sc_TournamentId tid, sc_AdminId aid, sc_PlayerId pid);

/**
 * Confirms a player for the match result
 * false on error
 */
bool rid_confirm_player(sc_RoundId self, sc_TournamentId tid, sc_AdminId aid, sc_PlayerId pid);

/**
 * Gets the players that are in a round who have confirmed
 * Returns NULL on error
 */
const sc_PlayerId *rid_confirmed_players(sc_RoundId rid, sc_TournamentId tid);

/**
 * Gets the players that are in a round who have confirmed
 * Returns NULL on error
 */
const sc_PlayerId *rid_confirmed_players(sc_RoundId self, sc_TournamentId tid);

/**
 * Returns the draw count for a game
 * Returns -1 on error
 */
int32_t rid_draws(sc_RoundId rid, sc_TournamentId tid);

/**
 * Returns the draw count for a game
 * Returns -1 on error
 */
int32_t rid_draws(sc_RoundId self, sc_TournamentId tid);

/**
 * Returns the total duration in a round
 * Retrusn -1 on error
 */
int64_t rid_duration(sc_RoundId rid, sc_TournamentId tid);

/**
 * Returns the total duration in a round
 * Retrusn -1 on error
 */
int64_t rid_duration(sc_RoundId self, sc_TournamentId tid);

/**
 * Kills a match
 * false on error
 */
bool rid_kill(sc_RoundId rid, sc_TournamentId tid, sc_AdminId aid);

/**
 * Kills a match
 * false on error
 */
bool rid_kill(sc_RoundId self, sc_TournamentId tid, sc_AdminId aid);

/**
 * Gets the round number
 * -1 on error
 */
int64_t rid_match_number(sc_RoundId rid, sc_TournamentId tid);

/**
 * Gets the round number
 * -1 on error
 */
int64_t rid_match_number(sc_RoundId self, sc_TournamentId tid);

/**
 * Gets the players that are in a round
 * Returns NULL on error
 */
const sc_PlayerId *rid_players(sc_RoundId rid, sc_TournamentId tid);

/**
 * Gets the players that are in a round
 * Returns NULL on error
 */
const sc_PlayerId *rid_players(sc_RoundId self, sc_TournamentId tid);

/**
 * Records draws for a round
 */
bool rid_record_draws(sc_RoundId rid, sc_TournamentId tid, sc_AdminId aid, uint32_t draws);

/**
 * Records draws for a round
 */
bool rid_record_draws(sc_RoundId self, sc_TournamentId tid, sc_AdminId aid, uint32_t draws);

/**
 * Records results for a round; DO NOT RECORD DRAWS HERE (it breaks :( )
 * false on error
 */
bool rid_record_result(sc_RoundId rid, sc_TournamentId tid, sc_AdminId aid, sc_PlayerId pid, uint32_t wins);

/**
 * Records results for a round; DO NOT RECORD DRAWS HERE (it breaks :( )
 * false on error
 */
bool rid_record_result(sc_RoundId self, sc_TournamentId tid, sc_AdminId aid, sc_PlayerId pid, uint32_t wins);

/**
 * Returns the result for a player in a round
 * Returns -1 on error
 */
int32_t rid_result_for(sc_RoundId rid, sc_TournamentId tid, sc_PlayerId pid);

/**
 * Returns the result for a player in a round
 * Returns -1 on error
 */
int32_t rid_result_for(sc_RoundId self, sc_TournamentId tid, sc_PlayerId pid);

/**
 * Gets the status for a round
 * Dead on error
 */
sc_RoundStatus rid_status(sc_RoundId rid, sc_TournamentId tid);

/**
 * Gets the status for a round
 * Dead on error
 */
sc_RoundStatus rid_status(sc_RoundId self, sc_TournamentId tid);

/**
 * Gets the table number
 * Warning: Currently cannot detect if the round has not been allocated a table number
 * -1 on error
 */
int64_t rid_table_number(sc_RoundId rid, sc_TournamentId tid);

/**
 * Gets the table number
 * Warning: Currently cannot detect if the round has not been allocated a table number
 * -1 on error
 */
int64_t rid_table_number(sc_RoundId self, sc_TournamentId tid);

/**
 * Grants a time extension of ext seconds to the round
 * false on error
 */
bool rid_time_extend(sc_RoundId rid, sc_TournamentId tid, sc_AdminId aid, uint64_t ext);

/**
 * Grants a time extension of ext seconds to the round
 * false on error
 */
bool rid_time_extend(sc_RoundId self, sc_TournamentId tid, sc_AdminId aid, uint64_t ext);

/**
 * Returns the amount of time left in a round
 * Retrusn -1 on error
 */
int64_t rid_time_left(sc_RoundId rid, sc_TournamentId tid);

/**
 * Returns the amount of time left in a round
 * Retrusn -1 on error
 */
int64_t rid_time_left(sc_RoundId self, sc_TournamentId tid);

/**
 * Saves a tournament to a name
 * Returns true if successful, false if not.
 */
bool save_tourn(sc_TournamentId tid, const char *__file);

/**
 * Saves a tournament to a name
 * Returns true if successful, false if not.
 */
bool save_tourn(sc_TournamentId self, const char *__file);

/**
 * Deallocates a block assigned in the FFI portion,
 * use this when handling with squire strings
 */
void sq_free(void *pointer, size_t len);

/**
 * Deallocates a block assigned in the FFI portion,
 * use this when handling with squire strings
 */
void sq_free(void *pointer, size_t len);

/**
 * Adds an admin to a local tournament in a way that is perfect for
 * adding the system user.
 */
bool tid_add_admin_local(sc_TournamentId tid, const char *__name, sc_AdminId aid, sc_SquireAccountId uid);

/**
 * Adds an admin to a local tournament in a way that is perfect for
 * adding the system user.
 */
bool tid_add_admin_local(sc_TournamentId self, const char *__name, sc_AdminId aid, sc_SquireAccountId uid);

/**
 * Adds a player to a tournament
 * On error a NULL UUID is returned
 */
sc_PlayerId tid_add_player(sc_TournamentId tid, const char *__name);

/**
 * Adds a player to a tournament
 * On error a NULL UUID is returned
 */
sc_PlayerId tid_add_player(sc_TournamentId self, const char *__name);

/**
 * Cancels a tournament
 * false on error, true on success.
 */
bool tid_cancel(sc_TournamentId tid, sc_AdminId aid);

/**
 * Cancels a tournament
 * false on error, true on success.
 */
bool tid_cancel(sc_TournamentId self, sc_AdminId aid);

/**
 * Drops a player for the tournament
 * On error false is returned
 */
bool tid_drop_player(sc_TournamentId tid, sc_PlayerId pid, sc_AdminId aid);

/**
 * Drops a player for the tournament
 * On error false is returned
 */
bool tid_drop_player(sc_TournamentId self, sc_PlayerId pid, sc_AdminId aid);

/**
 * End a tournament
 * false on error, true on success.
 */
bool tid_end(sc_TournamentId tid, sc_AdminId aid);

/**
 * End a tournament
 * false on error, true on success.
 */
bool tid_end(sc_TournamentId self, sc_AdminId aid);

/**
 * Returns the format of a tournament
 * Returns NULL if an error happens
 * This is heap allocated, please free it
 */
const char *tid_format(sc_TournamentId tid);

/**
 * Returns the format of a tournament
 * Returns NULL if an error happens
 * This is heap allocated, please free it
 */
const char *tid_format(sc_TournamentId self);

/**
 * Freezes a tournament
 * false on error, true on success.
 */
bool tid_freeze(sc_TournamentId tid, sc_AdminId aid);

/**
 * Freezes a tournament
 * false on error, true on success.
 */
bool tid_freeze(sc_TournamentId self, sc_AdminId aid);

/**
 * Returns the game size
 * -1 is the error value
 */
int32_t tid_game_size(sc_TournamentId tid);

/**
 * Returns the game size
 * -1 is the error value
 */
int32_t tid_game_size(sc_TournamentId self);

/**
 * Returns the max deck count
 * -1 is the error value
 */
int32_t tid_max_deck_count(sc_TournamentId tid);

/**
 * Returns the max deck count
 * -1 is the error value
 */
int32_t tid_max_deck_count(sc_TournamentId self);

/**
 * Returns the min deck count
 * -1 is the error value
 */
int32_t tid_min_deck_count(sc_TournamentId tid);

/**
 * Returns the min deck count
 * -1 is the error value
 */
int32_t tid_min_deck_count(sc_TournamentId self);

/**
 * Returns the name of a tournament
 * Returns NULL if an error happens
 * This is heap allocated, please free it
 */
const char *tid_name(sc_TournamentId tid);

/**
 * Returns the name of a tournament
 * Returns NULL if an error happens
 * This is heap allocated, please free it
 */
const char *tid_name(sc_TournamentId self);

/**
 * Pairs a set of rounds
 * Returns a null terminated list of the round ids
 * Returns NULL on error
 */
const sc_RoundId *tid_pair_round(sc_TournamentId tid, sc_AdminId aid);

/**
 * Pairs a set of rounds
 * Returns a null terminated list of the round ids
 * Returns NULL on error
 */
const sc_RoundId *tid_pair_round(sc_TournamentId self, sc_AdminId aid);

/**
 * Returns the pairing type
 * This is of type TournamentPreset, but an int to let me return error values
 * -1 is error value
 */
int32_t tid_pairing_type(sc_TournamentId tid);

/**
 * Returns the pairing type
 * This is of type TournamentPreset, but an int to let me return error values
 * -1 is error value
 */
int32_t tid_pairing_type(sc_TournamentId self);

/**
 * Returns a raw pointer to players
 * This is an array that is terminated by the NULL UUID
 * This is heap allocted, please free it
 * Returns NULL on error
 */
const sc_PlayerId *tid_players(sc_TournamentId tid);

/**
 * Returns a raw pointer to players
 * This is an array that is terminated by the NULL UUID
 * This is heap allocted, please free it
 * Returns NULL on error
 */
const sc_PlayerId *tid_players(sc_TournamentId self);

/**
 * Whether reg is open
 * False on error
 */
bool tid_reg_open(sc_TournamentId tid);

/**
 * Whether reg is open
 * False on error
 */
bool tid_reg_open(sc_TournamentId self);

/**
 * Whether checkins are needed
 * False on error
 */
bool tid_require_check_in(sc_TournamentId tid);

/**
 * Whether checkins are needed
 * False on error
 */
bool tid_require_check_in(sc_TournamentId self);

/**
 * Whether deck reg is needed
 * False on error
 */
bool tid_require_deck_reg(sc_TournamentId tid);

/**
 * Whether deck reg is needed
 * False on error
 */
bool tid_require_deck_reg(sc_TournamentId self);

/**
 * Returns the round length
 * -1 on error
 */
int64_t tid_round_length(sc_TournamentId tid);

/**
 * Returns the round length
 * -1 on error
 */
int64_t tid_round_length(sc_TournamentId self);

/**
 * Generates a round slip for a tournament
 * NULL on error
 */
const char *tid_round_slips_html(sc_TournamentId tid, const char *__css);

/**
 * Generates a round slip for a tournament
 * NULL on error
 */
const char *tid_round_slips_html(sc_TournamentId self, const char *__css);

/**
 * Returns a raw pointer to rounds
 * This is an array that is terminated by the NULL UUID
 * This is heap allocted, please free it
 * Returns NULL on error
 */
const sc_RoundId *tid_rounds(sc_TournamentId tid);

/**
 * Returns a raw pointer to rounds
 * This is an array that is terminated by the NULL UUID
 * This is heap allocted, please free it
 * Returns NULL on error
 */
const sc_RoundId *tid_rounds(sc_TournamentId self);

/**
 * TournamentIds can be used to get data safely from
 * the Rust lib with these methods
 * Returns a raw pointer to a list of standings
 * This is an array, the last element has a NULL player id
 * This is heap allocated, please free it
 * Returns NULL on error
 */
const sc_PlayerScore<sc_StandardScore> *tid_standings(sc_TournamentId tid);

/**
 * Returns a raw pointer to a list of standings
 * This is an array, the last element has a NULL player id
 * This is heap allocated, please free it
 * Returns NULL on error
 */
const sc_PlayerScore<sc_StandardScore> *tid_standings(sc_TournamentId self);

/**
 * Starts a tournament
 * false on error, true on success.
 */
bool tid_start(sc_TournamentId tid, sc_AdminId aid);

/**
 * Starts a tournament
 * false on error, true on success.
 */
bool tid_start(sc_TournamentId self, sc_AdminId aid);

/**
 * Returns the starting table number
 * Retruns -1 on error
 */
int32_t tid_starting_table_number(sc_TournamentId tid);

/**
 * Returns the starting table number
 * Retruns -1 on error
 */
int32_t tid_starting_table_number(sc_TournamentId self);

/**
 * Returns the status
 * Returns cancelled on error
 */
sc_TournamentStatus tid_status(sc_TournamentId tid);

/**
 * Returns the status
 * Returns cancelled on error
 */
sc_TournamentStatus tid_status(sc_TournamentId self);

/**
 * Defrosts a tournament
 * false on error, true on success.
 */
bool tid_thaw(sc_TournamentId tid, sc_AdminId aid);

/**
 * Defrosts a tournament
 * false on error, true on success.
 */
bool tid_thaw(sc_TournamentId self, sc_AdminId aid);

/**
 * Updates the settings
 * Values that are not to be changed should remain the
 * current setting, that would be the value the user
 * selected in the GUI so that is fine.
 * All input must be non-null.
 *
 * If any errors occur then all actions are rolled back
 * and, false returned.
 *
 * Otherwise true is returned and the operations are all
 * applied to the tournament.
 */
bool tid_update_settings(sc_TournamentId tid, const char *__format, uint64_t starting_table_number, bool use_table_number, uint8_t game_size, uint8_t min_deck_count, uint8_t max_deck_count, uint64_t match_length, bool reg_open, bool require_check_in, bool require_deck_reg, sc_AdminId aid);

/**
 * Updates the settings
 * Values that are not to be changed should remain the
 * current setting, that would be the value the user
 * selected in the GUI so that is fine.
 * All input must be non-null.
 *
 * If any errors occur then all actions are rolled back
 * and, false returned.
 *
 * Otherwise true is returned and the operations are all
 * applied to the tournament.
 */
bool tid_update_settings(sc_TournamentId self, const char *__format, uint64_t starting_table_number, bool use_table_number, uint8_t game_size, uint8_t min_deck_count, uint8_t max_deck_count, uint64_t match_length, bool reg_open, bool require_check_in, bool require_deck_reg, sc_AdminId aid);

/**
 * Returns whether table numbers are being used for this tournament
 * false, is the error value (kinda sketchy)
 */
bool tid_use_table_number(sc_TournamentId tid);

/**
 * Returns whether table numbers are being used for this tournament
 * false, is the error value (kinda sketchy)
 */
bool tid_use_table_number(sc_TournamentId self);

} // extern "C"

} // namespace squire_ffi

/* AUTO GENERATED SquireFFI BINDINGS DO NOT MODIFY BY HAND */

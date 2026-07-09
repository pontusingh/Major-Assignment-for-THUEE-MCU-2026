#pragma once
#include <Arduino.h>

// ---------- Tunable constants, straight from the spec ----------
// Centralizing these means Phase 2 logic reads names, not magic numbers,
// and any tuning later is a one-line change instead of a grep.
namespace PetConfig {
  constexpr int MOOD_MIN = 1;
  constexpr int MOOD_MAX = 10;
  constexpr int MOOD_DEFAULT = 5;

  constexpr unsigned long MOOD_DECAY_INTERVAL_MS   = 30000;  // -1 mood every 30s
  constexpr unsigned long FULL_DURATION_MS         = 120000; // Full -> Hungry after 2 min
  constexpr unsigned long PAT_COOLDOWN_MS          = 10000;  // repeat-pat lockout window
  constexpr unsigned long SLEEP_DIM_THRESHOLD_MS   = 5000;   // continuous dim -> sleep mode
  constexpr unsigned long STATUS_PRINT_INTERVAL_MS = 5000;   // debug print cadence

  // Phase 4 constant, unused until then:
  constexpr unsigned long DEEP_SLEEP_INACTIVITY_MS = 30000;  // no input -> deep sleep
}

enum class ActionMode : uint8_t {
  NONE    = 0,
  PLAY    = 1,
  IDLE    = 2,
  TIRED   = 3,
  FORWARD = 4
};

struct PetState {
  // --- Mood system (spec 2.2) ---
  int moodValue = PetConfig::MOOD_DEFAULT;
  unsigned long lastMoodDecayMs = 0;

  // --- Feeding system (spec 2.2.1) ---
  bool isFull = false;
  unsigned long fullStateStartMs = 0;

  // --- Motion / action system (spec 2.3) ---
  ActionMode currentAction = ActionMode::NONE;
  unsigned long actionStartMs = 0;
  unsigned long actionDurationMs = 0;

  // --- Interaction (spec 2.4) ---
  unsigned long lastPatMs = 0; // drives the 10s pat cooldown

  // --- Sleep mode: compulsory, light-based (spec 2.5) ---
  // This is a *behavioral* mode, MCU stays fully powered.
  bool sleepMode = false;
  unsigned long dimSinceMs = 0; // 0 = not currently dim; else timestamp dimness started

  // --- Debug / command system (spec 2.6) ---
  unsigned long lastStatusPrintMs = 0;

  // ===== Phase 4 fields (Low-power + Flash elective) =====
  // Live in the same struct on purpose, so one save/load pair covers
  // everything once wired up. Inert until Phase 4 — do not populate yet.
  bool wasSleeping = false;         // distinguishes deep-sleep wake from power-on reset
  unsigned long lastInteractMs = 0; // drives the 30s deep-sleep inactivity timeout
  uint32_t patCount = 0;            // Flash-persisted "memory" counters
  uint32_t feedCount = 0;
  uint32_t playCount = 0;

  // ===== Phase 5 (Network HTTP elective) =====
  // wifiConnected, lastFetchMs, cached weather string, etc.
  // Deliberately not declared yet — added when that phase starts.
};

// Single global instance.
//   Phase 2-3: declare plain       ->  PetState state;
//   Phase 4:   promote to RTC mem  ->  RTC_DATA_ATTR PetState state;
// (Note for later: RTC_DATA_ATTR default member initializers only apply
//  on power-on/brownout reset, not on wake-from-deep-sleep — worth a
//  real look when Phase 4 starts, not solved here.)
extern PetState state;

// --- Behavior module interface, implemented incrementally per phase ---
void resetState();    // Phase 2: apply defaults on power-on reset
void saveToFlash();   // Phase 4: write pet_cfg + pet_mem namespaces
void loadFromFlash();  // Phase 4: restore on boot

// Multi-threaded tower defence demo constants
// James Kayes ©2016 
//////////////////////////////////////////////
#pragma once
// Following google c++ guide for naming constants:
const float kPi = 3.14159265f;

const int kInitialMoney = 200000;
const int kInitialHealth = 25;

const float kTankSpeed = 100.0f;
const float kTankHitWidth = 21.0f;
const float kTankHitHeight = 24.0f;
const int kTankInitialHealth = 50;
const float kTankInitialSpawnTime = 6.0f; // Seconds before first tank spawns.
const int kTankIncome = 125;  // Money gained for destroying a tank.
const float kTankRespawnTime = 0.9f; // Seconds between tank respawns.

const int kTurretBuyCost = 250;
const int kTurretSellCost = 150;
const float kTurretRangeSquared = 4608.0f; // If a tank enters this radius around a turret, it aims and shoots at it.
const float kTurretRotationSpeed = 540.0f; // Degrees per second.
const float kTurretShootRate = 5.5f; // Shots per second.
const float kTurretAimAngle = 2.5f; // Lowering this number will increase turret accuracy.
const float kTurretInRangeEdgeFactor = 5.0f; // The edge length used by the pathfinding algorithm around turrets (so it can prefer paths out of range).

const float kShotSpeed = 200.0f;
const int kShotDamage = 8;
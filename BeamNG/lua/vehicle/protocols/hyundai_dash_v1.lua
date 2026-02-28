-- This Source Code Form is subject to the terms of the bCDDL, v. 1.1.
-- If a copy of the bCDDL was not distributed with this
-- file, You can obtain one at http://beamng.com/bCDDL-1.1.txt


-- generic outgauge implementation based on LiveForSpeed
local M = {}

local function init() end
local function reset() end
local function getAddress()        return "127.0.0.1" end
local function getPort()           return 9532 end
local function getMaxUpdateRate()  return 10 end  -- 10 Hz

local function isPhysicsStepUsed()
  return false
end

local function getStructDefinition()
  -- the original protocol documentation can be found at LFS/docs/InSim.txt
  return [[
	short		ignition;		// 0, 1, 2, 3
    short		speed;			// Wheel speed in km/h
    short		rpm;			// RPM
	short		gear;			// P, R, N, D, S, M
	short		gearIndex;		// 1-7 etc
    short		engTemp;		// *C
    short       showLights;		// Dash lights currently switched on
	short		idleRPM;		// idle rpm of current vehicle
    short       maxRPM;         // max rpm of current vehicle
	short		fuelCapacity;	// Fuel capacity in liters
	short		fuelVolume;		// Fuel volume in liters
	short       throttle;       // Throttle output in game
	short       clutch;         // Clutch output in game
  ]]
end



-- DL_x - bits for dashLights and showLights
local DL_LOHIBEAM	  = 2 ^ 0    -- low beam
local DL_HIGHBEAM     = 2 ^ 1    -- high beam
local DL_CHECKENG     = 2 ^ 2    -- check engine
local DL_OILWARN      = 2 ^ 3    -- oil pressure warning
local DL_HANDBRAKE    = 2 ^ 4    -- handbrake
local DL_TC           = 2 ^ 5    -- tc active or switched off
local DL_SIGNAL_L     = 2 ^ 6    -- left turn signal
local DL_SIGNAL_R     = 2 ^ 7    -- right turn signal
local DL_FOG	      = 2 ^ 8    -- fog light
local DL_CRUISE       = 2 ^ 9    -- cruise control
local DL_ABS          = 2 ^ 10   -- ABS active
local DL_SRS          = 2 ^ 11   -- SRS light

local cluster_max_rpm = 8000

local function fillStruct(o, dtSim)
  if not electrics.values.watertemp or not electrics.values.fuel then
    -- vehicle not completly initialized or not a vehicle, skip sending package
    return
  end

  local mps_to_kmh = 3.6

  o.ignition = electrics.values.ignitionLevel or 0
  o.speed = math.max(math.floor((electrics.values.wheelspeed or 0) * mps_to_kmh + 0.5), 0)
  o.rpm = math.max(math.floor(electrics.values.rpmTacho or 0), 0)
  o.gear = string.byte(electrics.values.gear) or 0
  o.gearIndex = electrics.values.gearIndex or 0
  o.engTemp = math.max(math.floor(electrics.values.watertemp or 0), 0)
  o.idleRPM = electrics.values.idlerpm or 0
  o.maxRPM = electrics.values.maxrpm or cluster_max_rpm
  o.fuelCapacity = math.floor((electrics.values.fuelCapacity or 0)*100) or 0 -- e.g 60, send 6000
  o.fuelVolume = math.floor((electrics.values.fuelVolume or 0)*100) or 0 -- e.g 54.2538, send 5425
  o.throttle = math.floor((electrics.values.throttle or 0)*100) or 0
  o.clutch = math.floor((electrics.values.clutch or 0)*100) or 0

  -- the lights
  if electrics.values.lowhighbeam   ~= 0 then o.showLights = bit.bor(o.showLights, DL_LOHIBEAM ) end
  if electrics.values.highbeam      ~= 0 then o.showLights = bit.bor(o.showLights, DL_HIGHBEAM ) end
  if electrics.values.checkengine   ~= false then o.showLights = bit.bor(o.showLights, DL_CHECKENG ) end
  if electrics.values.oil           ~= 0 then o.showLights = bit.bor(o.showLights, DL_OILWARN  ) end
  if electrics.values.parkingbrake  ~= 0 then o.showLights = bit.bor(o.showLights, DL_HANDBRAKE) end
  if electrics.values.hasESC then
	if (electrics.values.esc         == 1 or electrics.values.tcs == 1) then o.showLights = bit.bor(o.showLights, DL_TC) end
  end
  if electrics.values.signal_L      ~= 0 then o.showLights = bit.bor(o.showLights, DL_SIGNAL_L ) end
  if electrics.values.signal_R      ~= 0 then o.showLights = bit.bor(o.showLights, DL_SIGNAL_R ) end
  if electrics.values.fog           ~= 0 then o.showLights = bit.bor(o.showLights, DL_FOG ) end
  if (electrics.values.cruiseControlActive or 0)    ~= 0 then o.showLights = bit.bor(o.showLights, DL_CRUISE ) end
  if electrics.values.abs           ~= 0 then o.showLights = bit.bor(o.showLights, DL_ABS ) end
  if (damageTracker.getDamage("engine", "radiatorLeak") or false) ~= false then o.showLights = bit.bor(o.showLights, DL_SRS ) end
  
end

M.init = init
M.reset = reset
M.getAddress = getAddress
M.getPort = getPort
M.getMaxUpdateRate = getMaxUpdateRate
M.getStructDefinition = getStructDefinition
M.fillStruct = fillStruct
M.isPhysicsStepUsed = isPhysicsStepUsed

return M

function script.windowMain(dt)
  ui.text('Hyundai Dash V1')
end

local socket = require("shared/socket")
local udp = socket.udp()
udp:settimeout(0)
local udp_host = "127.0.0.1"
local udp_port = 9533

-- DL_x - bits for showLights
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


-- helper to encode a 16-bit unsigned integer in little-endian
local function pack_uint16_le(n)
    local low = n % 256
    local high = math.floor(n / 256)
    return string.char(low, high)
end

local timer = 0

function script.update(dt)
    timer = timer + dt
    if timer < 0.05 then return end -- cap to 20 Hz
    timer = 0

    local car = ac.getCar(0)
    if not car then return end

    local showLights = 0

    local ignition = 2
    local speedKMH = math.abs(math.floor(car.speedKmh))
    local rpm = math.abs(math.floor(car.rpm))
    local gear = math.floor(car.gear)
    local coolantTemp = math.abs(math.floor(car.waterTemperature)) or 90
    local idleRpm = math.abs(math.floor(car.rpmMinimum)) or 0
    local maxRpm = math.abs(math.floor(car.rpmLimiter)) or 8000
    local fuelCapacity = math.floor((car.maxFuel or 0) * 100)
    local fuelVolume = math.floor((car.fuel or 0) * 100)

    local oilPres = math.floor(car.oilPressure or 0)
    local oilTemp = math.floor(car.oilTemperature or 0)

    local throttle = math.floor(car.gas * 100)
    local clutch = math.floor(car.clutch * 100)
    local handbrake = math.floor(car.handbrake * 100)

    if car.headlightsActive then showLights = bit.bor(showLights, DL_LOHIBEAM) end
    if car.highBeams then showLights = bit.bor(showLights, DL_HIGHBEAM) end
    if car.engineLifeLeft <= 800 then showLights = bit.bor(showLights, DL_CHECKENG) end
    if oilPres < 1 or oilTemp > 130 then showLights = bit.bor(showLights, DL_OILWARN) end
    if handbrake > 0 then showLights = bit.bor(showLights, DL_HANDBRAKE) end
    if car.tractionControlInAction or car.tractionControlMode == 0 then showLights = bit.bor(showLights, DL_TC) end
    if car.turningLeftLights and car.turningLightsActivePhase then showLights = bit.bor(showLights, DL_SIGNAL_L) end
    if car.turningRightLights and car.turningLightsActivePhase then showLights = bit.bor(showLights, DL_SIGNAL_R) end
    if car.hazardLights and car.turningLightsActivePhase then
        showLights = bit.bor(showLights, DL_SIGNAL_L)
        showLights = bit.bor(showLights, DL_SIGNAL_R)
    end
    if car.absInAction then showLights = bit.bor(showLights, DL_ABS) end

    local gearIndex
    local gearDisplay

    if gear == -1 then
        gearDisplay = 82 -- R
        gearIndex = 0
    elseif gear == 0 then
        gearDisplay = 78 -- N
        gearIndex = 0
    elseif gear > 0 and car.autoShift then
        gearDisplay = 68 -- D
        gearIndex = gear
    else
        gearDisplay = gear
        gearIndex = gear
    end

    local values = {
    ignition, speedKMH, rpm, gearDisplay, gearIndex, coolantTemp,
    showLights, idleRpm, maxRpm, fuelCapacity, fuelVolume, throttle, clutch
    }
    local packet = ""
    for i = 1, #values do
        packet = packet .. pack_uint16_le(values[i])
    end

    udp:sendto(packet, udp_host, udp_port)

end
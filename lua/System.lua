function NewSystem (this, world)
    table.insert (world.systems, this)
    this.segments = {}
    local R1 = this.radius
    for _,orbit in ipairs (this.orbits) do
        local R2 = R1 + orbit.width
        local A = math.pi * 2 / orbit.nCells
        for i = 1, orbit.nCells do
            local segment = {
                parent = this,
                colour = Vec3.Random (true),
                R1 = R1,
                R2 = R2,
                A = A,
                B = A * (i - 1),
            }
            table.insert (world.segments, segment)
            table.insert (this.segments, segment)
        end
        R1 = R2
    end
end

function System (this)
    local function X (angle) return angle % (2 * math.pi) end
    local function Y (angle) return X (angle) < math.pi end
    for _,s in ipairs (this.segments) do
        s.phi = s.B + s.A * World.turn.float
    end

    V = Mat4.Inverse (this.tMat) % Lock
    R = Vec3.Length (V) 
    A = math.atan2 (V.e.y, V.e.x)
    if A < 0 then A = A + 2 * math.pi end
    for _,s in ipairs (this.segments) do
        if Y (A - s.phi) and Y (s.phi + s.A - A) and
                s.R1 < R and R < s.R2 then
            Selected[s] = true
        end
    end
end

function Segment (this)
    local colour = Selected[this] and colour.white or this.colour
    Core.Segment (this.parent.tMat, colour,
        this.R1, this.R2, this.A, this.phi)
end


function NewWorld ()
    world = PrepareWorld ()

    world.camera = DefaultCamera ()
    world.center = {tMat = mat4.id}

    local P1 = {
        colour = vec3 (1, 1, 1),
        radius = 1,
        orbits = {
            {width = 3, nSectors = 3},
            {width = 1, nSectors = 7},
        }
    }
    P1 = NewSystem (P1, world)

    local P2 = {
        external = {ring = P1.rings[1], 4},
        colour = vec3 (1, 1, 1),
        radius = 1,
        orbits = {
            {width = 1, nSectors = 3},
        }
    }
    P2 = NewSystem (P2, world)

	local C1 = {
		parent = P1.rings[1],
		rMat = mat4.Movement (vec3 (-0.4, 0, 0)),
		colour = colour.green,
		radius = 0.1,
	}
	table.insert (world.circles, C1)
	table.insert (world.nodes, C1)

    return world
end

function PrepareWorld ()
    local world = {}

    world.nodes = {}
    world.rings = {}
    world.systems = {}
    world.spheres = {}
    world.sectors = {}
    world.circles = {}

    world.turn = {
        int = 0,
        float = 0,
        endTime = 0
    }

    return world
end

// Stickwars 3D - Babylon.js Port
// Original C++ version by Michael Garland, 2003

class StickwarsGame {
    constructor() {
        this.canvas = document.getElementById('renderCanvas');
        this.engine = new BABYLON.Engine(this.canvas, true);

        // Terrain settings (matching original C++ values)
        this.terrainWidth = 100;
        this.terrainLength = 100;
        this.terrainScale = 1;
        this.heightData = [];

        // Movement settings
        this.walkSpeed = 0.5;
        this.runSpeed = 1.2;
        this.isRunning = false;

        // Input state
        this.keys = {};

        this.createScene();
        this.setupInput();
        this.startRenderLoop();
    }

    createScene() {
        this.scene = new BABYLON.Scene(this.engine);
        this.scene.clearColor = new BABYLON.Color3(0.65, 0.65, 0.65);

        // Create first-person camera
        this.camera = new BABYLON.UniversalCamera(
            'camera',
            new BABYLON.Vector3(50, 15, 50),
            this.scene
        );
        this.camera.minZ = 0.1;
        this.camera.maxZ = 5000;
        this.camera.fov = BABYLON.Tools.ToRadians(60);
        this.camera.attachControl(this.canvas, true);

        // Disable default camera controls - we'll use custom ones
        this.camera.inputs.clear();

        // Setup lighting (matching original C++ light position)
        const light = new BABYLON.HemisphericLight(
            'light',
            new BABYLON.Vector3(0, 1, 0),
            this.scene
        );
        light.intensity = 0.7;
        light.diffuse = new BABYLON.Color3(0.8, 0.9, 0.9);
        light.groundColor = new BABYLON.Color3(0.2, 0.4, 0.4);

        // Add directional light for shadows
        this.sunLight = new BABYLON.DirectionalLight(
            'sunLight',
            new BABYLON.Vector3(-1, -2, -1),
            this.scene
        );
        this.sunLight.position = new BABYLON.Vector3(50, 100, 50);
        this.sunLight.intensity = 0.5;

        // Create terrain
        this.createTerrain();

        // Add some initial hills (like the 'p' key grow mode in original)
        this.generateInitialTerrain();
    }

    createTerrain() {
        // Initialize height data array
        this.heightData = new Array(this.terrainWidth * this.terrainLength).fill(0);

        // Create ground mesh
        this.ground = BABYLON.MeshBuilder.CreateGround('ground', {
            width: this.terrainWidth * this.terrainScale,
            height: this.terrainLength * this.terrainScale,
            subdivisions: this.terrainWidth - 1,
            updatable: true
        }, this.scene);

        // Position terrain so it starts at origin
        this.ground.position.x = (this.terrainWidth * this.terrainScale) / 2;
        this.ground.position.z = (this.terrainLength * this.terrainScale) / 2;

        // Create grass material
        const groundMat = new BABYLON.StandardMaterial('groundMat', this.scene);
        groundMat.diffuseColor = new BABYLON.Color3(0.3, 0.6, 0.3);
        groundMat.specularColor = new BABYLON.Color3(0.1, 0.1, 0.1);
        this.ground.material = groundMat;
    }

    // Port of makeHill from heightMap.cpp
    makeHill(radius, xOrig, yOrig, smooth = 10) {
        const vertices_x_max = 2 * (radius - 1) + xOrig;
        const vertices_y_max = 2 * (radius - 1) + yOrig;

        for (let x = 0; x < radius; x++) {
            for (let y = 0; y < radius; y++) {
                let height = (radius * radius - ((x - radius) * (x - radius) + (y - radius) * (y - radius))) / smooth;

                if (height > 0) {
                    height = Math.sqrt(height);
                } else {
                    height = 0;
                }

                // Apply height to four quadrants (like original)
                this.addHeightAt(xOrig + x, yOrig + y, height);
                this.addHeightAt(vertices_x_max - x + 1, vertices_y_max - y + 1, height);
                this.addHeightAt(xOrig + x, vertices_y_max - y + 1, height);
                this.addHeightAt(vertices_x_max - x + 1, yOrig + y, height);
            }
        }

        this.updateTerrainMesh();
    }

    addHeightAt(x, z, height) {
        if (x > 0 && x < this.terrainLength && z > 0 && z < this.terrainWidth) {
            const index = x * this.terrainLength + z;
            if (index >= 0 && index < this.heightData.length) {
                this.heightData[index] += height;
            }
        }
    }

    updateTerrainMesh() {
        const positions = this.ground.getVerticesData(BABYLON.VertexBuffer.PositionKind);
        const subdivisions = this.terrainWidth - 1;

        for (let row = 0; row <= subdivisions; row++) {
            for (let col = 0; col <= subdivisions; col++) {
                const vertexIndex = (row * (subdivisions + 1) + col) * 3;
                const heightIndex = row * this.terrainLength + col;

                if (heightIndex < this.heightData.length) {
                    positions[vertexIndex + 1] = this.heightData[heightIndex];
                }
            }
        }

        this.ground.updateVerticesData(BABYLON.VertexBuffer.PositionKind, positions);

        // Recalculate normals for proper lighting
        const normals = [];
        BABYLON.VertexData.ComputeNormals(
            positions,
            this.ground.getIndices(),
            normals
        );
        this.ground.updateVerticesData(BABYLON.VertexBuffer.NormalKind, normals);
    }

    generateInitialTerrain() {
        // Generate several random hills like the original game
        for (let i = 0; i < 15; i++) {
            const radius = Math.floor(Math.random() * 20) + 5;
            const x = Math.floor(Math.random() * 60) + 20;
            const z = Math.floor(Math.random() * 60) + 20;
            const smooth = Math.random() * 20 + 10;
            this.makeHill(radius, x, z, smooth);
        }
    }

    // Get height at world position (port of getHeight from heightMap.cpp)
    getHeight(worldX, worldZ) {
        // Convert world position to terrain coordinates
        const x = worldX / this.terrainScale;
        const z = worldZ / this.terrainScale;

        if (x < 0 || x >= this.terrainWidth || z < 0 || z >= this.terrainLength) {
            return 0;
        }

        // Bilinear interpolation (like original)
        const x0 = Math.floor(x);
        const x1 = Math.ceil(x);
        const z0 = Math.floor(z);
        const z1 = Math.ceil(z);

        const h00 = this.heightData[x0 * this.terrainLength + z0] || 0;
        const h01 = this.heightData[x0 * this.terrainLength + z1] || 0;
        const h10 = this.heightData[x1 * this.terrainLength + z0] || 0;
        const h11 = this.heightData[x1 * this.terrainLength + z1] || 0;

        const fx = x - x0;
        const fz = z - z0;

        const h0 = h00 * (1 - fx) + h10 * fx;
        const h1 = h01 * (1 - fx) + h11 * fx;

        return h0 * (1 - fz) + h1 * fz;
    }

    setupInput() {
        // Keyboard input
        window.addEventListener('keydown', (e) => {
            this.keys[e.key.toLowerCase()] = true;
            if (e.key === 'Shift') this.isRunning = true;
        });

        window.addEventListener('keyup', (e) => {
            this.keys[e.key.toLowerCase()] = false;
            if (e.key === 'Shift') this.isRunning = false;
        });

        // Pointer lock for mouse look
        this.canvas.addEventListener('click', () => {
            this.canvas.requestPointerLock();
        });

        // Mouse look
        document.addEventListener('mousemove', (e) => {
            if (document.pointerLockElement === this.canvas) {
                // Horizontal rotation (yaw)
                this.camera.rotation.y += e.movementX * 0.002;

                // Vertical rotation (pitch) with clamping like original
                this.camera.rotation.x += e.movementY * 0.002;
                this.camera.rotation.x = Math.max(
                    -Math.PI / 2,
                    Math.min(Math.PI / 2, this.camera.rotation.x)
                );
            }
        });

        // Handle window resize
        window.addEventListener('resize', () => {
            this.engine.resize();
        });
    }

    updateMovement() {
        const speed = this.isRunning ? this.runSpeed : this.walkSpeed;

        // Get camera forward and right vectors (ignoring Y for ground movement)
        const forward = new BABYLON.Vector3(
            Math.sin(this.camera.rotation.y),
            0,
            Math.cos(this.camera.rotation.y)
        );
        const right = new BABYLON.Vector3(
            Math.sin(this.camera.rotation.y - Math.PI / 2),
            0,
            Math.cos(this.camera.rotation.y - Math.PI / 2)
        );

        let movement = BABYLON.Vector3.Zero();

        // WASD movement (matching original controls)
        if (this.keys['w']) movement.addInPlace(forward.scale(speed));
        if (this.keys['s']) movement.addInPlace(forward.scale(-speed));
        if (this.keys['a']) movement.addInPlace(right.scale(-speed));
        if (this.keys['d']) movement.addInPlace(right.scale(speed));

        // Apply movement
        this.camera.position.addInPlace(movement);

        // Clamp to terrain bounds (like original)
        const maxX = this.terrainWidth * this.terrainScale;
        const maxZ = this.terrainLength * this.terrainScale;

        this.camera.position.x = Math.max(0, Math.min(maxX, this.camera.position.x));
        this.camera.position.z = Math.max(0, Math.min(maxZ, this.camera.position.z));

        // Set camera height based on terrain + eye height (like original: height + 10)
        const terrainHeight = this.getHeight(this.camera.position.x, this.camera.position.z);
        this.camera.position.y = terrainHeight + 10;
    }

    startRenderLoop() {
        this.engine.runRenderLoop(() => {
            this.updateMovement();
            this.scene.render();
        });
    }
}

// Start the game when page loads
window.addEventListener('DOMContentLoaded', () => {
    new StickwarsGame();
});

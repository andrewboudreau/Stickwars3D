// Stickwars 3D - Babylon.js Port
// Original C++ version by Michael Garland, 2003
// Web port with skeletal animation system

// ============================================
// OBJ LOADER
// ============================================
class OBJLoader {
    static async load(url, scene) {
        const response = await fetch(url);
        const text = await response.text();
        return OBJLoader.parse(text, scene);
    }

    static parse(text, scene) {
        const lines = text.split('\n');
        const vertices = [];
        const faces = [];

        for (const line of lines) {
            const parts = line.trim().split(/\s+/);
            if (parts[0] === 'v') {
                vertices.push(new BABYLON.Vector3(
                    parseFloat(parts[1]),
                    parseFloat(parts[2]),
                    parseFloat(parts[3])
                ));
            } else if (parts[0] === 'f') {
                // OBJ faces are 1-indexed, convert to 0-indexed
                const faceIndices = [];
                for (let i = 1; i < parts.length; i++) {
                    const idx = parseInt(parts[i].split('/')[0]) - 1;
                    faceIndices.push(idx);
                }
                // Triangulate if more than 3 vertices
                for (let i = 1; i < faceIndices.length - 1; i++) {
                    faces.push(faceIndices[0], faceIndices[i], faceIndices[i + 1]);
                }
            }
        }

        // Create mesh from parsed data
        const mesh = new BABYLON.Mesh("objMesh", scene);
        const positions = [];
        const indices = [];

        for (const v of vertices) {
            positions.push(v.x, v.y, v.z);
        }

        for (const idx of faces) {
            indices.push(idx);
        }

        const vertexData = new BABYLON.VertexData();
        vertexData.positions = positions;
        vertexData.indices = indices;

        // Compute normals
        const normals = [];
        BABYLON.VertexData.ComputeNormals(positions, indices, normals);
        vertexData.normals = normals;

        vertexData.applyToMesh(mesh);
        return mesh;
    }
}

// ============================================
// KFM KEYFRAME PARSER
// ============================================
class KFMLoader {
    static async load(url) {
        const response = await fetch(url);
        const text = await response.text();
        return KFMLoader.parse(text);
    }

    static parse(text) {
        const lines = text.trim().split('\n');
        const numFrames = parseInt(lines[0]);
        const keyframes = [];

        for (let i = 1; i <= numFrames; i++) {
            const parts = lines[i].trim().split(/\s+/).map(parseFloat);
            let idx = 0;

            const keyframe = {
                time: parts[idx++],
                // Right arm
                r_shoulder: new BABYLON.Quaternion(parts[idx++], parts[idx++], parts[idx++], parts[idx++]),
                r_elbow: new BABYLON.Quaternion(parts[idx++], parts[idx++], parts[idx++], parts[idx++]),
                // Left arm
                l_shoulder: new BABYLON.Quaternion(parts[idx++], parts[idx++], parts[idx++], parts[idx++]),
                l_elbow: new BABYLON.Quaternion(parts[idx++], parts[idx++], parts[idx++], parts[idx++]),
                // Right leg
                r_hip: new BABYLON.Quaternion(parts[idx++], parts[idx++], parts[idx++], parts[idx++]),
                r_knee: new BABYLON.Quaternion(parts[idx++], parts[idx++], parts[idx++], parts[idx++]),
                // Left leg
                l_hip: new BABYLON.Quaternion(parts[idx++], parts[idx++], parts[idx++], parts[idx++]),
                l_knee: new BABYLON.Quaternion(parts[idx++], parts[idx++], parts[idx++], parts[idx++]),
                // Torso
                u_torso: new BABYLON.Quaternion(parts[idx++], parts[idx++], parts[idx++], parts[idx++]),
                l_torso: new BABYLON.Quaternion(parts[idx++], parts[idx++], parts[idx++], parts[idx++]),
                // Translation
                torso_tran: new BABYLON.Vector3(parts[idx++], parts[idx++], parts[idx++]),
                // Body root rotation
                body: new BABYLON.Quaternion(parts[idx++], parts[idx++], parts[idx++], parts[idx++])
            };

            keyframes.push(keyframe);
        }

        return keyframes;
    }
}

// ============================================
// ROBOT MESH MANAGER (loads all body parts)
// ============================================
class RobotMeshManager {
    constructor(scene) {
        this.scene = scene;
        this.meshes = {};
        this.loaded = false;
    }

    async loadAll() {
        const parts = ['torso', 'bicep', 'forearm', 'hand', 'thigh', 'calf', 'foot'];

        for (const part of parts) {
            try {
                const mesh = await OBJLoader.load(`robot/${part}.obj`, this.scene);
                mesh.setEnabled(false); // Template mesh, will be cloned
                this.meshes[part] = mesh;
            } catch (e) {
                console.error(`Failed to load ${part}.obj:`, e);
            }
        }

        this.loaded = true;
        return this.meshes;
    }

    clonePart(partName, newName) {
        if (!this.meshes[partName]) return null;
        return this.meshes[partName].clone(newName);
    }
}

// ============================================
// ROBOT CLASS (Animated Stick Figure)
// ============================================
class Robot {
    constructor(scene, meshManager, material) {
        this.scene = scene;
        this.meshManager = meshManager;
        this.material = material;

        // Animation state
        this.keyframes = [];
        this.currentFrame = 0;
        this.time = 0;
        this.playing = false;
        this.loop = true;
        this.animationSpeed = 1.0;

        // Root transform node
        this.root = new BABYLON.TransformNode("robotRoot", scene);

        // Body parts hierarchy
        this.parts = {};
        this.buildSkeleton();
    }

    buildSkeleton() {
        // Scale factor to make the model visible
        const scale = 0.5;

        // Body root (handles twist/lean/tilt)
        this.bodyRoot = new BABYLON.TransformNode("bodyRoot", this.scene);
        this.bodyRoot.parent = this.root;

        // Upper torso
        this.upperTorso = new BABYLON.TransformNode("upperTorso", this.scene);
        this.upperTorso.parent = this.bodyRoot;
        this.upperTorso.position.y = 6.5 * scale;

        // Create torso mesh
        this.parts.torso = this.meshManager.clonePart('torso', 'torsoMesh');
        if (this.parts.torso) {
            this.parts.torso.parent = this.upperTorso;
            this.parts.torso.scaling = new BABYLON.Vector3(scale, scale, scale);
            this.parts.torso.material = this.material;
            this.parts.torso.setEnabled(true);
        }

        // ===== RIGHT ARM =====
        this.rShoulderNode = new BABYLON.TransformNode("rShoulder", this.scene);
        this.rShoulderNode.parent = this.upperTorso;
        this.rShoulderNode.position = new BABYLON.Vector3(-1.2 * scale, 0, 0);

        this.parts.rBicep = this.meshManager.clonePart('bicep', 'rBicepMesh');
        if (this.parts.rBicep) {
            this.parts.rBicep.parent = this.rShoulderNode;
            this.parts.rBicep.scaling = new BABYLON.Vector3(scale, scale, scale);
            this.parts.rBicep.material = this.material;
            this.parts.rBicep.setEnabled(true);
        }

        this.rElbowNode = new BABYLON.TransformNode("rElbow", this.scene);
        this.rElbowNode.parent = this.rShoulderNode;
        this.rElbowNode.position = new BABYLON.Vector3(-1.75 * scale, 0, 0);

        this.parts.rForearm = this.meshManager.clonePart('forearm', 'rForearmMesh');
        if (this.parts.rForearm) {
            this.parts.rForearm.parent = this.rElbowNode;
            this.parts.rForearm.scaling = new BABYLON.Vector3(scale, scale, scale);
            this.parts.rForearm.material = this.material;
            this.parts.rForearm.setEnabled(true);
        }

        this.rHandNode = new BABYLON.TransformNode("rHand", this.scene);
        this.rHandNode.parent = this.rElbowNode;
        this.rHandNode.position = new BABYLON.Vector3(-1.5 * scale, 0, 0);

        this.parts.rHand = this.meshManager.clonePart('hand', 'rHandMesh');
        if (this.parts.rHand) {
            this.parts.rHand.parent = this.rHandNode;
            this.parts.rHand.scaling = new BABYLON.Vector3(scale, scale, scale);
            this.parts.rHand.material = this.material;
            this.parts.rHand.setEnabled(true);
        }

        // ===== LEFT ARM =====
        this.lShoulderNode = new BABYLON.TransformNode("lShoulder", this.scene);
        this.lShoulderNode.parent = this.upperTorso;
        this.lShoulderNode.position = new BABYLON.Vector3(1.2 * scale, 0, 0);

        this.parts.lBicep = this.meshManager.clonePart('bicep', 'lBicepMesh');
        if (this.parts.lBicep) {
            this.parts.lBicep.parent = this.lShoulderNode;
            this.parts.lBicep.scaling = new BABYLON.Vector3(-scale, scale, scale); // Mirror
            this.parts.lBicep.material = this.material;
            this.parts.lBicep.setEnabled(true);
        }

        this.lElbowNode = new BABYLON.TransformNode("lElbow", this.scene);
        this.lElbowNode.parent = this.lShoulderNode;
        this.lElbowNode.position = new BABYLON.Vector3(1.75 * scale, 0, 0);

        this.parts.lForearm = this.meshManager.clonePart('forearm', 'lForearmMesh');
        if (this.parts.lForearm) {
            this.parts.lForearm.parent = this.lElbowNode;
            this.parts.lForearm.scaling = new BABYLON.Vector3(-scale, scale, scale);
            this.parts.lForearm.material = this.material;
            this.parts.lForearm.setEnabled(true);
        }

        this.lHandNode = new BABYLON.TransformNode("lHand", this.scene);
        this.lHandNode.parent = this.lElbowNode;
        this.lHandNode.position = new BABYLON.Vector3(1.5 * scale, 0, 0);

        this.parts.lHand = this.meshManager.clonePart('hand', 'lHandMesh');
        if (this.parts.lHand) {
            this.parts.lHand.parent = this.lHandNode;
            this.parts.lHand.scaling = new BABYLON.Vector3(-scale, scale, scale);
            this.parts.lHand.material = this.material;
            this.parts.lHand.setEnabled(true);
        }

        // ===== LOWER TORSO (for legs) =====
        this.lowerTorso = new BABYLON.TransformNode("lowerTorso", this.scene);
        this.lowerTorso.parent = this.bodyRoot;
        this.lowerTorso.position.y = 3.0 * scale;

        // ===== RIGHT LEG =====
        this.rHipNode = new BABYLON.TransformNode("rHip", this.scene);
        this.rHipNode.parent = this.lowerTorso;
        this.rHipNode.position = new BABYLON.Vector3(-0.8 * scale, 0, 0);

        this.parts.rThigh = this.meshManager.clonePart('thigh', 'rThighMesh');
        if (this.parts.rThigh) {
            this.parts.rThigh.parent = this.rHipNode;
            this.parts.rThigh.scaling = new BABYLON.Vector3(scale, scale, scale);
            this.parts.rThigh.material = this.material;
            this.parts.rThigh.setEnabled(true);
        }

        this.rKneeNode = new BABYLON.TransformNode("rKnee", this.scene);
        this.rKneeNode.parent = this.rHipNode;
        this.rKneeNode.position = new BABYLON.Vector3(0, -2.0 * scale, 0);

        this.parts.rCalf = this.meshManager.clonePart('calf', 'rCalfMesh');
        if (this.parts.rCalf) {
            this.parts.rCalf.parent = this.rKneeNode;
            this.parts.rCalf.scaling = new BABYLON.Vector3(scale, scale, scale);
            this.parts.rCalf.material = this.material;
            this.parts.rCalf.setEnabled(true);
        }

        this.rFootNode = new BABYLON.TransformNode("rFoot", this.scene);
        this.rFootNode.parent = this.rKneeNode;
        this.rFootNode.position = new BABYLON.Vector3(0, -2.0 * scale, 0);

        this.parts.rFoot = this.meshManager.clonePart('foot', 'rFootMesh');
        if (this.parts.rFoot) {
            this.parts.rFoot.parent = this.rFootNode;
            this.parts.rFoot.scaling = new BABYLON.Vector3(scale, scale, scale);
            this.parts.rFoot.material = this.material;
            this.parts.rFoot.setEnabled(true);
        }

        // ===== LEFT LEG =====
        this.lHipNode = new BABYLON.TransformNode("lHip", this.scene);
        this.lHipNode.parent = this.lowerTorso;
        this.lHipNode.position = new BABYLON.Vector3(0.8 * scale, 0, 0);

        this.parts.lThigh = this.meshManager.clonePart('thigh', 'lThighMesh');
        if (this.parts.lThigh) {
            this.parts.lThigh.parent = this.lHipNode;
            this.parts.lThigh.scaling = new BABYLON.Vector3(-scale, scale, scale);
            this.parts.lThigh.material = this.material;
            this.parts.lThigh.setEnabled(true);
        }

        this.lKneeNode = new BABYLON.TransformNode("lKnee", this.scene);
        this.lKneeNode.parent = this.lHipNode;
        this.lKneeNode.position = new BABYLON.Vector3(0, -2.0 * scale, 0);

        this.parts.lCalf = this.meshManager.clonePart('calf', 'lCalfMesh');
        if (this.parts.lCalf) {
            this.parts.lCalf.parent = this.lKneeNode;
            this.parts.lCalf.scaling = new BABYLON.Vector3(-scale, scale, scale);
            this.parts.lCalf.material = this.material;
            this.parts.lCalf.setEnabled(true);
        }

        this.lFootNode = new BABYLON.TransformNode("lFoot", this.scene);
        this.lFootNode.parent = this.lKneeNode;
        this.lFootNode.position = new BABYLON.Vector3(0, -2.0 * scale, 0);

        this.parts.lFoot = this.meshManager.clonePart('foot', 'lFootMesh');
        if (this.parts.lFoot) {
            this.parts.lFoot.parent = this.lFootNode;
            this.parts.lFoot.scaling = new BABYLON.Vector3(-scale, scale, scale);
            this.parts.lFoot.material = this.material;
            this.parts.lFoot.setEnabled(true);
        }
    }

    setKeyframes(keyframes) {
        this.keyframes = keyframes;
        this.currentFrame = 0;
        this.time = keyframes.length > 0 ? keyframes[0].time : 0;
    }

    play(loop = true) {
        this.playing = true;
        this.loop = loop;
    }

    stop() {
        this.playing = false;
    }

    setPosition(x, y, z) {
        this.root.position.set(x, y, z);
    }

    setRotationY(angle) {
        this.root.rotation.y = angle;
    }

    update(deltaTime) {
        if (!this.playing || this.keyframes.length < 2) return;

        // Advance time
        this.time += deltaTime * this.animationSpeed * 60; // 60 = original game speed factor

        // Find current and next keyframe
        let currentKF = this.keyframes[this.currentFrame];
        let nextFrame = (this.currentFrame + 1) % this.keyframes.length;
        let nextKF = this.keyframes[nextFrame];

        // Check if we need to advance to next frame
        if (this.time >= nextKF.time) {
            this.currentFrame = nextFrame;
            if (this.currentFrame === 0) {
                // Looped back to start
                if (this.loop) {
                    this.time = this.keyframes[0].time;
                } else {
                    this.playing = false;
                    return;
                }
            }
            currentKF = this.keyframes[this.currentFrame];
            nextFrame = (this.currentFrame + 1) % this.keyframes.length;
            nextKF = this.keyframes[nextFrame];
        }

        // Calculate interpolation factor (0 to 1)
        const frameDuration = nextKF.time - currentKF.time;
        const t = frameDuration > 0 ? (this.time - currentKF.time) / frameDuration : 0;

        // Interpolate and apply transforms
        this.applyInterpolatedPose(currentKF, nextKF, t);
    }

    applyInterpolatedPose(kf1, kf2, t) {
        // Body root rotation
        const bodyRot = BABYLON.Quaternion.Slerp(kf1.body, kf2.body, t);
        this.bodyRoot.rotationQuaternion = bodyRot;

        // Torso translation
        const torsoTrans = BABYLON.Vector3.Lerp(kf1.torso_tran, kf2.torso_tran, t);
        this.bodyRoot.position = torsoTrans.scale(0.5); // Scale factor

        // Upper torso
        const uTorsoRot = BABYLON.Quaternion.Slerp(kf1.u_torso, kf2.u_torso, t);
        this.upperTorso.rotationQuaternion = uTorsoRot;

        // Lower torso
        const lTorsoRot = BABYLON.Quaternion.Slerp(kf1.l_torso, kf2.l_torso, t);
        this.lowerTorso.rotationQuaternion = lTorsoRot;

        // Right arm
        const rShoulderRot = BABYLON.Quaternion.Slerp(kf1.r_shoulder, kf2.r_shoulder, t);
        this.rShoulderNode.rotationQuaternion = rShoulderRot;

        const rElbowRot = BABYLON.Quaternion.Slerp(kf1.r_elbow, kf2.r_elbow, t);
        this.rElbowNode.rotationQuaternion = rElbowRot;

        // Left arm
        const lShoulderRot = BABYLON.Quaternion.Slerp(kf1.l_shoulder, kf2.l_shoulder, t);
        this.lShoulderNode.rotationQuaternion = lShoulderRot;

        const lElbowRot = BABYLON.Quaternion.Slerp(kf1.l_elbow, kf2.l_elbow, t);
        this.lElbowNode.rotationQuaternion = lElbowRot;

        // Right leg
        const rHipRot = BABYLON.Quaternion.Slerp(kf1.r_hip, kf2.r_hip, t);
        this.rHipNode.rotationQuaternion = rHipRot;

        const rKneeRot = BABYLON.Quaternion.Slerp(kf1.r_knee, kf2.r_knee, t);
        this.rKneeNode.rotationQuaternion = rKneeRot;

        // Left leg
        const lHipRot = BABYLON.Quaternion.Slerp(kf1.l_hip, kf2.l_hip, t);
        this.lHipNode.rotationQuaternion = lHipRot;

        const lKneeRot = BABYLON.Quaternion.Slerp(kf1.l_knee, kf2.l_knee, t);
        this.lKneeNode.rotationQuaternion = lKneeRot;
    }

    dispose() {
        for (const part of Object.values(this.parts)) {
            if (part) part.dispose();
        }
        this.root.dispose();
    }
}

// ============================================
// MAIN GAME CLASS
// ============================================
class StickwarsGame {
    constructor() {
        this.canvas = document.getElementById('renderCanvas');
        this.engine = new BABYLON.Engine(this.canvas, true);

        // Terrain settings
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

        // Robots
        this.robots = [];
        this.meshManager = null;
        this.animations = {};

        this.init();
    }

    async init() {
        this.createScene();
        this.setupInput();

        // Load robot meshes and animations
        await this.loadAssets();

        // Create some test robots
        this.createTestRobots();

        this.startRenderLoop();
    }

    createScene() {
        this.scene = new BABYLON.Scene(this.engine);
        this.scene.clearColor = new BABYLON.Color3(0.5, 0.7, 0.9);

        // Create first-person camera
        this.camera = new BABYLON.UniversalCamera(
            'camera',
            new BABYLON.Vector3(50, 20, 50),
            this.scene
        );
        this.camera.minZ = 0.1;
        this.camera.maxZ = 5000;
        this.camera.fov = BABYLON.Tools.ToRadians(60);
        this.camera.inputs.clear();

        // Hemispheric light
        const light = new BABYLON.HemisphericLight(
            'light',
            new BABYLON.Vector3(0, 1, 0),
            this.scene
        );
        light.intensity = 0.9;
        light.diffuse = new BABYLON.Color3(1, 1, 0.9);
        light.groundColor = new BABYLON.Color3(0.3, 0.3, 0.4);

        // Directional light for better definition
        const dirLight = new BABYLON.DirectionalLight(
            'dirLight',
            new BABYLON.Vector3(-1, -2, -1),
            this.scene
        );
        dirLight.intensity = 0.5;

        // Create terrain
        this.createTerrain();
        this.generateInitialTerrain();
    }

    async loadAssets() {
        // Load mesh templates
        this.meshManager = new RobotMeshManager(this.scene);
        await this.meshManager.loadAll();

        // Load animations
        try {
            this.animations.running = await KFMLoader.load('animations/running.kfm');
            this.animations.stab = await KFMLoader.load('animations/stab.kfm');
            this.animations.chop = await KFMLoader.load('animations/chop.kfm');
            this.animations.die = await KFMLoader.load('animations/die.kfm');
            console.log('Animations loaded:', Object.keys(this.animations));
        } catch (e) {
            console.error('Failed to load animations:', e);
        }
    }

    createTestRobots() {
        if (!this.meshManager.loaded) return;

        // Create materials for teams
        const redMaterial = new BABYLON.StandardMaterial("redMat", this.scene);
        redMaterial.diffuseColor = new BABYLON.Color3(0.9, 0.2, 0.2);
        redMaterial.specularColor = new BABYLON.Color3(0.3, 0.3, 0.3);

        const blueMaterial = new BABYLON.StandardMaterial("blueMat", this.scene);
        blueMaterial.diffuseColor = new BABYLON.Color3(0.2, 0.2, 0.9);
        blueMaterial.specularColor = new BABYLON.Color3(0.3, 0.3, 0.3);

        // Create several robots
        for (let i = 0; i < 5; i++) {
            const material = i % 2 === 0 ? redMaterial : blueMaterial;
            const robot = new Robot(this.scene, this.meshManager, material);

            // Position randomly on terrain
            const x = 30 + Math.random() * 40;
            const z = 30 + Math.random() * 40;
            const y = this.getHeight(x, z);
            robot.setPosition(x, y, z);
            robot.setRotationY(Math.random() * Math.PI * 2);

            // Set running animation
            if (this.animations.running) {
                robot.setKeyframes(this.animations.running);
                robot.play(true);
            }

            this.robots.push(robot);
        }

        console.log(`Created ${this.robots.length} robots`);
    }

    createTerrain() {
        this.heightData = new Array(this.terrainWidth * this.terrainLength).fill(0);

        this.ground = BABYLON.MeshBuilder.CreateGround('ground', {
            width: this.terrainWidth * this.terrainScale,
            height: this.terrainLength * this.terrainScale,
            subdivisions: this.terrainWidth - 1,
            updatable: true
        }, this.scene);

        this.ground.position.x = (this.terrainWidth * this.terrainScale) / 2;
        this.ground.position.z = (this.terrainLength * this.terrainScale) / 2;

        const groundMat = new BABYLON.StandardMaterial('groundMat', this.scene);
        groundMat.diffuseColor = new BABYLON.Color3(0.3, 0.6, 0.3);
        groundMat.specularColor = new BABYLON.Color3(0.1, 0.1, 0.1);
        this.ground.material = groundMat;
    }

    makeHill(radius, xOrig, yOrig, smooth = 10) {
        const vertices_x_max = 2 * (radius - 1) + xOrig;
        const vertices_y_max = 2 * (radius - 1) + yOrig;

        for (let x = 0; x < radius; x++) {
            for (let y = 0; y < radius; y++) {
                let height = (radius * radius - ((x - radius) * (x - radius) + (y - radius) * (y - radius))) / smooth;
                if (height > 0) height = Math.sqrt(height);
                else height = 0;

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
        const normals = [];
        BABYLON.VertexData.ComputeNormals(positions, this.ground.getIndices(), normals);
        this.ground.updateVerticesData(BABYLON.VertexBuffer.NormalKind, normals);
    }

    generateInitialTerrain() {
        for (let i = 0; i < 15; i++) {
            const radius = Math.floor(Math.random() * 20) + 5;
            const x = Math.floor(Math.random() * 60) + 20;
            const z = Math.floor(Math.random() * 60) + 20;
            const smooth = Math.random() * 20 + 10;
            this.makeHill(radius, x, z, smooth);
        }
    }

    getHeight(worldX, worldZ) {
        const x = worldX / this.terrainScale;
        const z = worldZ / this.terrainScale;
        if (x < 0 || x >= this.terrainWidth || z < 0 || z >= this.terrainLength) return 0;

        const x0 = Math.floor(x), x1 = Math.ceil(x);
        const z0 = Math.floor(z), z1 = Math.ceil(z);

        const h00 = this.heightData[x0 * this.terrainLength + z0] || 0;
        const h01 = this.heightData[x0 * this.terrainLength + z1] || 0;
        const h10 = this.heightData[x1 * this.terrainLength + z0] || 0;
        const h11 = this.heightData[x1 * this.terrainLength + z1] || 0;

        const fx = x - x0, fz = z - z0;
        const h0 = h00 * (1 - fx) + h10 * fx;
        const h1 = h01 * (1 - fx) + h11 * fx;
        return h0 * (1 - fz) + h1 * fz;
    }

    setupInput() {
        window.addEventListener('keydown', (e) => {
            this.keys[e.key.toLowerCase()] = true;
            if (e.key === 'Shift') this.isRunning = true;
        });

        window.addEventListener('keyup', (e) => {
            this.keys[e.key.toLowerCase()] = false;
            if (e.key === 'Shift') this.isRunning = false;
        });

        this.canvas.addEventListener('click', () => {
            this.canvas.requestPointerLock();
        });

        document.addEventListener('mousemove', (e) => {
            if (document.pointerLockElement === this.canvas) {
                this.camera.rotation.y += e.movementX * 0.002;
                this.camera.rotation.x += e.movementY * 0.002;
                this.camera.rotation.x = Math.max(-Math.PI / 2, Math.min(Math.PI / 2, this.camera.rotation.x));
            }
        });

        window.addEventListener('resize', () => this.engine.resize());
    }

    updateMovement() {
        const speed = this.isRunning ? this.runSpeed : this.walkSpeed;

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
        if (this.keys['w']) movement.addInPlace(forward.scale(speed));
        if (this.keys['s']) movement.addInPlace(forward.scale(-speed));
        if (this.keys['a']) movement.addInPlace(right.scale(-speed));
        if (this.keys['d']) movement.addInPlace(right.scale(speed));

        this.camera.position.addInPlace(movement);

        const maxX = this.terrainWidth * this.terrainScale;
        const maxZ = this.terrainLength * this.terrainScale;
        this.camera.position.x = Math.max(0, Math.min(maxX, this.camera.position.x));
        this.camera.position.z = Math.max(0, Math.min(maxZ, this.camera.position.z));

        const terrainHeight = this.getHeight(this.camera.position.x, this.camera.position.z);
        this.camera.position.y = terrainHeight + 10;
    }

    updateRobots(deltaTime) {
        for (const robot of this.robots) {
            robot.update(deltaTime);
        }
    }

    startRenderLoop() {
        let lastTime = performance.now();

        this.engine.runRenderLoop(() => {
            const now = performance.now();
            const deltaTime = (now - lastTime) / 1000;
            lastTime = now;

            this.updateMovement();
            this.updateRobots(deltaTime);
            this.scene.render();
        });
    }
}

// Start the game when page loads
window.addEventListener('DOMContentLoaded', () => {
    new StickwarsGame();
});

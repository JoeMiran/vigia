# 🌙 Guariteiro da Meia-Noite
> Jogo 3D de terror/comédia em OpenGL — Projeto de Computação Gráfica

---

## 📁 Estrutura de diretórios

```
guariteiro/
│
├── src/                        ← Código-fonte (.cpp)
│   ├── main.cpp                ← Ponto de entrada
│   ├── core/
│   │   ├── Game.cpp            ← Loop principal, estados do jogo
│   │   ├── Renderer.cpp        ← Toda a renderização OpenGL
│   │   └── InputManager.cpp    ← Teclado e mouse
│   ├── entities/
│   │   ├── Player.cpp          ← Jogador (movimento, lanterna, sanidade)
│   │   └── Enemy.cpp           ← Inimigos (TODO)
│   └── systems/
│       └── SceneManager.cpp    ← Cenas, spawn, lógica do turno
│
├── include/                    ← Headers (.h)
│   ├── core/
│   │   ├── Game.h
│   │   ├── Renderer.h
│   │   ├── InputManager.h
│   │   ├── Shader.h            ← Utilitário de shaders GLSL
│   │   └── Camera.h            ← Câmera FPS + suporte à lanterna
│   ├── entities/
│   │   ├── GameObject.h        ← Classe base (transform, update)
│   │   ├── Player.h
│   │   └── Enemy.h             ← (TODO)
│   └── systems/
│       └── SceneManager.h
│
├── shaders/                    ← GLSL
│   ├── scene_vertex.glsl       ← Vertex shader (MVP)
│   └── scene_fragment.glsl     ← Fragment shader (lanterna spotlight + noite)
│
├── assets/
│   ├── textures/               ← .jpg / .png das superfícies
│   ├── models/                 ← .obj / .fbx dos objetos 3D
│   ├── audio/                  ← efeitos sonoros e trilha
│   └── fonts/                  ← fontes para UI
│
├── docs/                       ← Documentação do projeto
├── scripts/                    ← Scripts de build auxiliares
├── CMakeLists.txt
├── .gitignore
└── README.md
```

---

## 🎮 Conceito do jogo

Você é um **guarita de segurança** num turno noturno assombrado.
Use sua **lanterna** para iluminar o ambiente — mas tome cuidado,
pois quanto mais você vê, mais a sua **sanidade** decresce.

| Mecânica         | Descrição                                          |
|------------------|----------------------------------------------------|
| 🔦 Lanterna      | Spotlight FPS, liga/desliga com `F`                |
| 🏃 Corrida       | `Shift` para correr, consome stamina               |
| 🧠 Sanidade      | Reduz ao ver inimigos, afeta efeitos visuais       |
| 🚧 Barreira      | Acionar/levantar com `E`                           |
| 📋 Rondas        | Eventos escalantes a cada hora do turno            |

---

## 📦 Dependências

### Linux (Ubuntu/Debian)
```bash
sudo apt install cmake build-essential libglfw3-dev libglm-dev libgl1-mesa-dev
```

### Windows
```bash
vcpkg install glfw3 glm glad
```

### macOS
```bash
brew install cmake glfw glm
```

### GLAD (loader OpenGL)
1. Acesse https://glad.dav1d.de/
2. Language: **C/C++** | Spec: **OpenGL** | Profile: **Core** | Version: **3.3**
3. Baixe, copie `glad/include/` e `glad/src/glad.c` para o projeto

### stb_image (texturas)
```bash
curl -o include/stb_image.h \
  https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
```

---

## 🚀 Build

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
./guariteiro
```

---

## 🎮 Controles

| Tecla       | Ação                    |
|-------------|-------------------------|
| W A S D     | Mover                   |
| Shift       | Correr                  |
| Mouse       | Olhar                   |
| F           | Ligar/desligar lanterna |
| E           | Interagir               |
| P           | Pausar                  |
| ESC         | Sair                    |

---

## 🗺️ Roadmap

- [x] Estrutura base do projeto
- [x] Câmera FPS
- [x] Lanterna (spotlight) com penumbra
- [x] Sistema de sanidade e stamina
- [x] Cena inicial (guarita, barreira, chão)
- [ ] Carregar modelos .obj (Assimp)
- [ ] Inimigos com pathfinding básico
- [ ] Efeitos de baixa sanidade (vinheta, distorção)
- [ ] Sistema de áudio (OpenAL ou miniaudio)
- [ ] UI (barra de sanidade e stamina)
- [ ] Skybox noturna
- [ ] Sistema de save/eventos por hora

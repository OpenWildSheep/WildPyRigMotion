
# PyRigLogic

PyRigLogic is a project that builds **RigLogic** and provides a Python binding for it. It is designed to be used in software like **Blender**.

## 📖 Overview

[**RigLogic**](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Plugins/RigLogicLib/riglogic) is part of [**Unreal Engine**](https://www.unrealengine.com) and [**MetaHuman**](https://www.unrealengine.com/fr/metahuman). It is used to manage advanced character rigs.

⚠️ **PyRigLogic does not contain the source code of RigLogic.** You must have access to the [**Unreal Engine source code**](https://www.unrealengine.com/fr/ue-on-github) to build PyRigLogic.

### More about metahuman and RigLogic:
- https://www.youtube.com/watch?v=QXGRq7a6ZqA
- https://cdn2.unrealengine.com/rig-logic-whitepaper-v2-5c9f23f7e210.pdf

## 🚀 Installation

### 📌 Requirements

-   **Unreal Engine** (Source code required)
-   **CMake** (Version 3.13+ recommended)
-   **Python** (Version 3.7+ recommended)

### 🛠️ Compilation
    
Run the following commands, replacing `path/to/UnrealEngine` and `path/to/python` with your actual paths:
    
    ```sh
    cmake -B build -DRIGLOGIC_SOURCE_DIR="path/to/UnrealEngine/Engine/Plugins/Animation/RigLogic/Source/RigLogicLib" -DPYTHON_EXECUTABLE="path/to/python/python.exe"
    cmake --build build --config Release
    ```    

## 📚 Usage

Once PyRigLogic is installed, here is an example of how to use it:

```python
from PyRigLogic import BinaryStreamReader, FileStream, DataLayer  
  
stream = FileStream("ada.dna", FileStream.AccessMode.Read, FileStream.OpenMode.Binary)  
reader = BinaryStreamReader(stream, DataLayer.All)  
reader.read()  
  
from PyRigLogic import RigInstance, RigLogic  
  
rig_logic = RigLogic(reader)  
rig_instance = RigInstance(rig_logic)  
  
expressions = ["browDown", "browLateral"]  
  
rig_instance.setLOD(0)  
  
for expression_name in expressions:  
    for raw_control_index in range(reader.getRawControlCount()):  
        control_name = reader.getRawControlName(raw_control_index)  
        if expression_name in control_name.split(".")[-1]:  
            rig_instance.setRawControl(raw_control_index, 1.0)  
  
rig_logic.calculate(rig_instance)  
joint_value = rig_instance.getJointOutputs()  
```
## 📜 License

This project is licensed under the **Boost Software License 1.0**. See the `LICENSE` file for more details.

## 🤝 Contributing

Contributions are welcome! To contribute:

1.  Fork the project 🍴
2.  Create a feature branch: `git checkout -b feature/my-new-feature`
3.  Commit your changes: `git commit -m "Added a new feature"`
4.  Push the branch: `git push origin my-new-feature`
5.  Open a pull request ✅

## 💡 Acknowledgments

Thanks to **Epic Games** for Unreal Engine and Metahuman. 🎮🚀

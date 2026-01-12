# ARVision
ARVision is a C++ project to integrate and develop augmented reality applications. It is a modular framework which is ready to work and deploy on different operation systems and devices. The main focus is on the integration from virtual objects into a camera livestream. The goal is to have a tool, which can provide information about orientation, layers and movement in a stream in real time for an application to decide which objects should be visible and on which location and rotation the user should see it.

<img width="782" height="371" alt="arv_base_structure drawio" src="https://github.com/user-attachments/assets/4724e317-c2b8-4556-9194-4938df9fa179" />


## Structure

```
ARVision/
├─ arv_core_interfaces/
├─ arv_core/
├─ providers/
│  ├─ plattform_providers/
│  ├─ rendering_api_providers/
│  └─ logging_provider/
├─ sandbox/
└─ arv_studio/
```

### ARV-Core-Interfaces
This main „interface only“ library is a collection of all classes which should be visible to use, configure or override in one of the client applications (e.g. Sandbox or ARV-Studio). All of these classes are important for the ARV-Core library and should be treated as an abstract representation for purposes like rendering, os depended logic and logging. 

### ARV Core
The kernel library of the framework. This library handles all the business logic for rendering, calculating and visualizing. It is limited to a minimum amount of external libraries. Nevertheless
It should be easy to compile for all devices (e.g. MacOS, IOS, Android or Windows). To achieve this it needs some mandatory providers. These providers are not part of the ARV-Core, but responsible for platform dependent logic. 
Another important feature of the ARV-Core library is it’s possible to override core functionalities from client side if needed. It works similar to singleton classes in java spring boot applications. All of theses core features are implemented in singleton classes and registered in an internal control layer. Of course all of them have default implementations, but it is possible to override it by reimplementing/exend the specific class and register a new instance of the singleton in the core module.

### Providers
As already described in the ARV Core section, providers are mandatory platform specific implementations for the Core library. This folder is a collection of some already implemented providers which are free to use in real applications. Of course they are not needed in case it is not matching to the selected platform.

<img width="452" height="452" alt="arv_module_logic_detail drawio" src="https://github.com/user-attachments/assets/8254b751-358e-428b-93b5-da45358de1d8" />


### Sandbox
A sandbox application to develop core features of the framework.

### ARV-Studio
This application is the first ready-to-use application with the ARV-Core library. It is a desktop application (currently Mac only), implemented with OpenGL, GLFW and ImGui. The goal ist to have a tool which provides test and training data for augmented reality applications. The user should be able to create training data in a virtual room with rendered objects. These data (and a stream of the capturing) can be exported to be uses in other applications for model training (e.g. PyTorch scripts).
Also it should be possible to select a real video stream (external or internal) as test data.
Later the ARV-Studio can also be used to test implemented C++ modules for production ready augmented reality applications.

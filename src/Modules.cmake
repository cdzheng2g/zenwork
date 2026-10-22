# Each target has an explicit dependency boundary. Runtime targets do not link Widgets.
add_library(hans_contracts INTERFACE)
target_include_directories(hans_contracts INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hans_contracts INTERFACE Qt${QT_VERSION_MAJOR}::Core)

add_library(hans_rules STATIC
    modules/rules/RuleRegistry.cpp
    modules/rules/BuiltinRules.cpp
    app/registration/rules/RegisterRules.cpp
)
target_link_libraries(hans_rules PUBLIC hans_contracts)

add_library(hans_inference STATIC infrastructure/inference/rknn/RknnSession.cpp)
target_link_libraries(hans_inference PUBLIC hans_contracts)
target_include_directories(hans_inference PRIVATE ${CMAKE_SOURCE_DIR}/third_party/rknn/include)

add_library(hans_operators STATIC
    application/InspectionRunner.cpp
    application/operators/OperatorRegistry.cpp
    application/operators/OperatorGeometry.cpp
    application/operators/OperatorResources.cpp
    application/vision/ImageInput.cpp
    application/vision/ImageTransform.cpp
    application/vision/DetectionResults.cpp
    application/vision/detection/DetectionDecoderRegistry.cpp
    application/vision/detection/DetectionEngine.cpp

    modules/operators/ocr/OcrOperator.cpp
    modules/operators/ocr/OcrEngine.cpp
    modules/operators/ocr/OcrModule.cpp
    modules/operators/template_match/TemplateOperator.cpp
    modules/operators/template_match/TemplateModule.cpp
    modules/operators/brightness/BrightnessModule.cpp
    modules/operators/det/DetModule.cpp
    modules/operators/det/decoders/yolov5/YoloV5Decoder.cpp
    
    app/registration/vision/RegisterOperators.cpp
    app/registration/vision/RegisterDetectionDecoders.cpp
    modules/operators/template_match/TemplateMatchCorrector.cpp
    domain/project/LogicEvaluator.cpp
)
target_link_libraries(hans_operators PUBLIC hans_contracts)

add_library(hans_protocols STATIC
    infrastructure/communication/TcpServerService.cpp
    infrastructure/communication/ModbusTcpOutputService.cpp
    infrastructure/communication/ModbusTcpServerService.cpp
    infrastructure/communication/TcpResultPublisher.cpp
    application/communication/ResultOutputDispatcher.cpp
    application/communication/ResultDeliveryTracker.cpp
    app/registration/communication/RegisterCommunication.cpp
    modules/communication/tcp/TcpModule.cpp
    modules/communication/modbus/ModbusClientModule.cpp
    modules/communication/modbus/ModbusServerModule.cpp
)
target_link_libraries(hans_protocols PUBLIC hans_contracts)

add_library(hans_schemes STATIC
    application/scheme/SchemeCatalog.cpp
    application/scheme/SchemePackage.cpp
    application/scheme/SchemeAssetStore.cpp
)
target_link_libraries(hans_schemes PUBLIC hans_contracts)

add_library(hans_runtime STATIC
    application/inspection/InspectionService.cpp
    application/inspection/InspectionCoordinator.cpp
    application/scheme/SchemeActivationService.cpp
    infrastructure/inference/InspectionProcess.cpp
    infrastructure/camera/CameraRecovery.cpp
    application/InternalTrigger.cpp
    application/communication/SchemeCommunicationRuntime.cpp
    infrastructure/persistence/InspectionArchive.cpp
    infrastructure/persistence/ArchiveQueue.cpp
)
target_link_libraries(hans_runtime PUBLIC hans_contracts)

add_library(hans_parameter_ui STATIC
    ui/components/NumericParameterField.cpp
    ui/components/ParameterForm.cpp
    ui/components/FileParameterField.cpp
)
target_link_libraries(hans_parameter_ui PUBLIC hans_contracts)

add_library(hans_reference STATIC infrastructure/reference/ReferenceImageManager.cpp)
target_link_libraries(hans_reference PUBLIC hans_contracts ${OpenCV_LIBS})

add_library(hans_operator_editors STATIC
    app/registration/ui/RegisterOperatorEditors.cpp
    ui/operators/RuleSetEditor.cpp
    ui/operators/ImageOperatorEditor.cpp
)
target_link_libraries(hans_operator_editors PUBLIC hans_contracts)

target_link_libraries(hans_operators PUBLIC hans_rules hans_inference ${OpenCV_LIBS})
target_link_libraries(hans_protocols PUBLIC Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::SerialBus)
target_link_libraries(hans_schemes PUBLIC hans_yaml hans_operators hans_protocols)
target_link_libraries(hans_runtime PUBLIC hans_operators hans_protocols hans_schemes Qt${QT_VERSION_MAJOR}::Concurrent)
target_link_libraries(hans_parameter_ui PUBLIC Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(hans_operator_editors PUBLIC hans_parameter_ui hans_operators hans_reference)

# Convert bundled PPOCRv4 ONNX models with rknn-toolkit2 and onnx 1.16.2.
# Input is UINT8 RGB NHWC; normalization is embedded in the RKNN models.
from rknn.api import RKNN
from pathlib import Path
root=Path(__file__).resolve().parents[1] / 'resource/models/ocr'
for name,mean,std in [('rec',[127.5]*3,[127.5]*3),('det',[123.675,116.28,103.53],[58.395,57.12,57.375])]:
 r=RKNN(verbose=False)
 r.config(mean_values=[mean],std_values=[std],target_platform='rk3576')
 assert r.load_onnx(model=str(root/f'ppocrv4_{name}.onnx'))==0
 assert r.build(do_quantization=False)==0
 assert r.export_rknn(str(root/f'ppocrv4_{name}.rknn'))==0
 r.release()
 print('EXPORTED',name,flush=True)

//
//  KerWXObject+DeviceScanCode.m
//  KerWX
//
//  Created by zuowu on 2018/12/19.
//  Copyright © 2018 kkmofang.cn. All rights reserved.
//

#import "KerWXObject+DeviceScanCode.h"
#import <AVFoundation/AVFoundation.h>

#define WXScanCodeKey "WXScanCodeKey"

@implementation WXScanCodeRes

-(instancetype) initWithReadableCodeObject:(AVMetadataMachineReadableCodeObject *) object{

    if (self = [super init]) {

        self.result = object.stringValue;
        self.scanType = [object.type ker_scanType];
        //暂时还不知道怎么计算编码格式先填写空白
        self.charSet = @"UTF-8";
        //path是微信特定path 现在还不知道要如何兼容
        self.path = @"";
        
        NSData *data =[object.stringValue dataUsingEncoding:NSUTF8StringEncoding];
        self.rawData = [data base64EncodedStringWithOptions:0];
        
    }
    return self;
}

-(void)dealloc{
    NSLog(@"[WXScanCodeRes] [dealloc]");
}

@end

@interface ScanLine : UIView

@end

@implementation ScanLine

-(void)drawRect:(CGRect)rect{
    CGFloat colors [] = {
        0.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 0.0
    };
    CGFloat locations[] = {
        0.0f,0.5f,1.0f
    };
    
    CGColorSpaceRef baseSpace = CGColorSpaceCreateDeviceRGB();
    CGGradientRef gradient = CGGradientCreateWithColorComponents(baseSpace, colors, locations, sizeof(colors)/(sizeof(colors[0])*4));
    CGColorSpaceRelease(baseSpace);
    baseSpace = NULL;
    
    CGContextRef context = UIGraphicsGetCurrentContext();

    CGContextSaveGState(context);                // SaveGState
    CGContextClipToRect(context, rect);
    
    CGPoint startPoint = CGPointMake(CGRectGetMinX(rect),CGRectGetMidY(rect));
    CGPoint endPoint = CGPointMake(CGRectGetMaxX(rect), CGRectGetMidY(rect));
    
    CGContextDrawLinearGradient(context, gradient, startPoint, endPoint, 0);
    
    CGGradientRelease(gradient);
    gradient = NULL;
    
    CGContextRestoreGState(context);            // RestoreGState
    CGContextDrawPath(context, kCGPathStroke);
}

@end

@interface ScanView : UIView

@end

@implementation ScanView

-(void)drawRect:(CGRect)rect{

    CGContextRef context = UIGraphicsGetCurrentContext();
    CGMutablePathRef path = CGPathCreateMutable();
    
    //左上
    CGPathMoveToPoint(path, nil, 0, 0);
    CGPathAddLineToPoint(path, nil, 0, 25);
    CGPathMoveToPoint(path, nil, 0, 0);
    CGPathAddLineToPoint(path, nil, 25, 0);
    
    //右上
    CGPathMoveToPoint(path, nil, rect.size.width, 0);
    CGPathAddLineToPoint(path, nil, rect.size.width - 25, 0);
    CGPathMoveToPoint(path, nil, rect.size.width, 0);
    CGPathAddLineToPoint(path, nil, rect.size.width, 25);
    
    //左下
    CGPathMoveToPoint(path, nil, 0, rect.size.height);
    CGPathAddLineToPoint(path, nil, 0, rect.size.height - 25);
    CGPathMoveToPoint(path, nil, 0, rect.size.height);
    CGPathAddLineToPoint(path, nil, 25, rect.size.height);
    
    //右下
    CGPathMoveToPoint(path, nil, rect.size.width, rect.size.height);
    CGPathAddLineToPoint(path, nil, rect.size.width - 25, rect.size.height);
    CGPathMoveToPoint(path, nil, rect.size.width, rect.size.height);
    CGPathAddLineToPoint(path, nil, rect.size.width, rect.size.height - 25);
    
    CGContextAddPath(context, path);
    
    CGContextSetRGBStrokeColor(context, 0.0f, 1.0f, 0.0f, 1.0f); //设置笔触颜色
    CGContextSetRGBFillColor(context, 0.0f, 1.0f, 0.0f, 1.0f);   //设置填充色
    CGContextSetLineWidth(context, 6.0);                          //设置线条宽度
    
    CGContextDrawPath(context, kCGPathFillStroke);
    CGPathRelease(path);
}

@end

@interface WXScanCodeViewController : UIViewController <AVCaptureMetadataOutputObjectsDelegate, UINavigationControllerDelegate, UIImagePickerControllerDelegate>

@property (strong, nonatomic) KerJSObject * kerScanCodeObject;

@property (strong, nonatomic) AVCaptureDevice * device;                    //采集的设备
@property (strong, nonatomic) AVCaptureDeviceInput * input;                //设备的输入
@property (strong, nonatomic) AVCaptureMetadataOutput * output;           //输出
@property (strong, nonatomic) AVCaptureSession * session;                  //采集流
@property (strong, nonatomic) AVCaptureVideoPreviewLayer * previewLayer;  //窗口

@property (strong, nonatomic) ScanView * scanView;
@property (strong, nonatomic) ScanLine * scanLine;               //扫描线

@end

@implementation WXScanCodeViewController



-(instancetype) initWithObject:(KerJSObject *) object{
    if (self= [super init]) {
        
        id<WXScanCodeObject> v = [object implementProtocol:@protocol(WXScanCodeObject)];
        if (!v.onlyFromCamera){
            self.navigationItem.rightBarButtonItem = [[UIBarButtonItem alloc] initWithTitle:@"相册" style:UIBarButtonItemStylePlain target:self action:@selector(btnPhotoAlbum)];
        }
        self.kerScanCodeObject = object;
    }
    return self;
}

-(void)viewDidLoad{
    [super viewDidLoad];
    
    //scan ui
    CGFloat ScreenWidth = [[UIScreen mainScreen] bounds].size.width;
    CGFloat screenHeight = [[UIScreen mainScreen] bounds].size.height;
    CGFloat imageX = ScreenWidth*0.15;
    CGFloat imageY = screenHeight*0.25;
    
    UIView * maskView = [[UIView alloc] initWithFrame:CGRectMake(0, 0, self.view.frame.size.width, self.view.frame.size.height)];
    maskView.backgroundColor = [UIColor colorWithWhite:0.0 alpha:0.5];
    [self.view addSubview:maskView];

    UIBezierPath * maskPath = [UIBezierPath bezierPathWithRect:self.view.bounds];
    [maskPath appendPath:[[UIBezierPath bezierPathWithRect:CGRectMake(imageX, imageY, ScreenWidth*0.7, ScreenWidth*0.7)] bezierPathByReversingPath]];
    CAShapeLayer * maskLayer = [[CAShapeLayer alloc] init];
    maskLayer.path = maskPath.CGPath;
    maskView.layer.mask = maskLayer;
    
    _scanView = [[ScanView alloc] initWithFrame:CGRectMake(imageX, imageY, ScreenWidth*0.7, ScreenWidth*0.7)];
    _scanView.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0];
    [self.view addSubview:_scanView];
    
    _scanLine = [[ScanLine alloc] initWithFrame:CGRectMake(0, 5, _scanView.frame.size.width, 2)];
    _scanLine.backgroundColor = [UIColor colorWithRed:0 green:0 blue:0 alpha:0];
    [_scanView addSubview:_scanLine];
    
    [self startAnimation];
    [self startScan];
    
}

-(AVCaptureDevice *)device{
    if (_device == nil) {
        _device = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    }
    return _device;
}

-(AVCaptureDeviceInput *)input{
    if (_input == nil) {
        NSError * error;
        _input = [AVCaptureDeviceInput deviceInputWithDevice:self.device error:&error];
        if (error) {
            //发出错误
            if (self.kerScanCodeObject) {
                WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"scanCode:error init AVCaptureDeviceInpute fail !"];
                id<WXScanCodeObject> v = [self.kerScanCodeObject implementProtocol:@protocol(WXScanCodeObject)];
                [v success:res];
                [v fail:res];
            }
            return nil;
        }
    }
    return _input;
}

-(AVCaptureMetadataOutput *)output{
    if (_output == nil) {
        _output = [[AVCaptureMetadataOutput alloc]init];
        [_output setMetadataObjectsDelegate:self queue:dispatch_get_main_queue()];
    }
    return _output;
}

-(AVCaptureSession *)session{
    if (_session == nil) {
        _session = [[AVCaptureSession alloc]init];
        [_session setSessionPreset:AVCaptureSessionPresetHigh];
        [_session addInput:self.input];
        [_session addOutput:self.output];
    }
    return _session;
}

-(AVCaptureVideoPreviewLayer *)previewLayer{
    if (_previewLayer == nil) {
        _previewLayer = [AVCaptureVideoPreviewLayer layerWithSession:self.session];
        _previewLayer.videoGravity=AVLayerVideoGravityResizeAspectFill;
    }
    return _previewLayer;
}

-(void)startAnimation{
    
    [UIView animateWithDuration:2 animations:^{
        self.scanLine.frame = CGRectMake(0, self.scanView.frame.size.height - 7, self.scanView.frame.size.width, 2);
    } completion:^(BOOL finished) {
        if (finished) {
            self.scanLine.frame =  CGRectMake(0, 5, self.scanView.frame.size.width, 2);
            [self startAnimation];
        }
    }];
}

-(void)stopAnimation{
    [self.scanLine.layer removeAllAnimations];
}

-(NSArray<NSString *> *)scanTypes:(NSArray<NSString *> *)types{
    
    NSArray * output = @[];
    
    for (NSString * type in types) {
        
        if ([type isEqualToString:@"barCode"]) {
            
            output = [output arrayByAddingObjectsFromArray:@[
                                                             AVMetadataObjectTypeAztecCode,
                                                             AVMetadataObjectTypeCode39Code,
                                                             AVMetadataObjectTypeCode93Code,
                                                             AVMetadataObjectTypeCode128Code,
                                                             AVMetadataObjectTypeEAN8Code,
                                                             AVMetadataObjectTypeEAN13Code,
                                                             AVMetadataObjectTypeITF14Code,
                                                             AVMetadataObjectTypeUPCECode,
                                                             ]];
            
        }else if ([type isEqualToString:@"qrCode"]){
            
            output = [output arrayByAddingObjectsFromArray:@[AVMetadataObjectTypeQRCode]];
            
        }else if ([type isEqualToString:@"datamatrix"]){
            
            output = [output arrayByAddingObjectsFromArray:@[AVMetadataObjectTypeDataMatrixCode]];
            
        }else if ([type isEqualToString:@"pdf417"]){
            
            output = [output arrayByAddingObjectsFromArray:@[AVMetadataObjectTypePDF417Code]];
            
        }
    }
    
    return output;
}

-(void)startScan{
    
    if (self.kerScanCodeObject) {
        
        id<WXScanCodeObject> v = [self.kerScanCodeObject implementProtocol:@protocol(WXScanCodeObject)];
        self.previewLayer.frame = self.view.bounds;
        
        if (v.scanType == nil || v.scanType.count == 0) {
            self.output.metadataObjectTypes = [self scanTypes:@[@"barCode",@"qrCode"]];
        }else {
            self.output.metadataObjectTypes = [self scanTypes:v.scanType];
        }
        
        [self.view.layer insertSublayer:self.previewLayer atIndex:0];
        
        CGFloat ScreenWidth = [[UIScreen mainScreen] bounds].size.width;
        CGFloat screenHeight = [[UIScreen mainScreen] bounds].size.height;
        
        _output.rectOfInterest = CGRectMake(0.15, 0.25, 0.7, ScreenWidth * 0.7 / screenHeight);
        
        [self.session startRunning];
        
    }

}

-(void)btnPhotoAlbum{
    
    if (![UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypePhotoLibrary]){
        //相册权限没开 返回错误信息
        if (self.kerScanCodeObject) {
            WXCallbackRes * res = [[WXCallbackRes alloc] initWithErrMsg:@"scanCode:error no album permissions!"];
            id<WXScanCodeObject> v = [self.kerScanCodeObject implementProtocol:@protocol(WXScanCodeObject)];
            [v success:res];
            [v fail:res];
        }
        return;
    }
    
    UIImagePickerController *ipc = [[UIImagePickerController alloc] init];
    ipc.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
    ipc.allowsEditing = YES;
    ipc.delegate = self;
    [self presentViewController:ipc animated:YES completion:nil];
}

#pragma mark - AVCaptureMetadataOutputObjects delegate

-(void)captureOutput:(AVCaptureOutput *)captureOutput didOutputMetadataObjects:(NSArray *)metadataObjects fromConnection:(AVCaptureConnection *)connection{
    
    [self stopAnimation];

    if (metadataObjects.count > 0) {
        [self.session stopRunning];
        AVMetadataMachineReadableCodeObject * metadataObject = [metadataObjects objectAtIndex:0];
        if (self.kerScanCodeObject) {
            WXScanCodeRes * res = [[WXScanCodeRes alloc] initWithReadableCodeObject:metadataObject];
            id<WXScanCodeObject> v = [self.kerScanCodeObject implementProtocol:@protocol(WXScanCodeObject)];
            [v success:res];
            [v complete:res];
        }
    }
    
    UIViewController * topViewController = [[UIApplication sharedApplication] keyWindow].rootViewController;
    if ([topViewController isKindOfClass:[UINavigationController class]]) {
        UINavigationController * nav = (UINavigationController *)topViewController;
        [nav popViewControllerAnimated:YES];
    }
    
}

#pragma mark -- UIImagePickerControllerDelegate --

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary<NSString *,id> *)info{
    
    // 销毁控制器
    [picker dismissViewControllerAnimated:YES completion:^{
        
        // 设置图片
        UIImage *image = info[UIImagePickerControllerOriginalImage];
        if(image){
            //1. 初始化扫描仪，设置设别类型和识别质量
            CIDetector*detector = [CIDetector detectorOfType:CIDetectorTypeQRCode context:nil options:@{ CIDetectorAccuracy : CIDetectorAccuracyHigh }];
            //2. 扫描获取的特征组
            NSArray *features = [detector featuresInImage:[CIImage imageWithCGImage:image.CGImage]];
            if(features && [features count] > 0){
                //3. 获取扫描结果
                CIQRCodeFeature *feature = [features objectAtIndex:0];
                
                WXScanCodeRes * res = [[WXScanCodeRes alloc] init];
                res.result = feature.messageString;
                res.scanType = @"QR_CODE";
                res.charSet = @"UTF-8";
                res.path = @"";
                NSData *data =[feature.messageString dataUsingEncoding:NSUTF8StringEncoding];
                res.rawData = [data base64EncodedStringWithOptions:0];
                
                id<WXScanCodeObject> v = [self.kerScanCodeObject implementProtocol:@protocol(WXScanCodeObject)];
                [v success:res];
                [v complete:res];
                
                
                UIViewController * topViewController = [[UIApplication sharedApplication] keyWindow].rootViewController;
                if ([topViewController isKindOfClass:[UINavigationController class]]) {
                    UINavigationController * nav = (UINavigationController *)topViewController;
                    [nav popViewControllerAnimated:YES];
                }
                
            }else {
                
                UIAlertController * actionSheet = [UIAlertController alertControllerWithTitle:@"扫描结果" message:@"没有发现二维码" preferredStyle:UIAlertControllerStyleAlert];
                UIAlertAction * action1 = [UIAlertAction actionWithTitle:@"确定" style:UIAlertActionStyleDefault handler:nil];
                [actionSheet addAction:action1];
                [self presentViewController:actionSheet animated:YES completion:nil];
                
            }
        }
        
    }];

}

@end

@implementation KerWXObject (DeviceScanCode)

-(void) scanCode:(KerJSObject *) object{

    WXScanCodeViewController * scanCodeViewController = [[WXScanCodeViewController alloc] initWithObject:object];
    UIViewController * topViewController = [[UIApplication sharedApplication] keyWindow].rootViewController;
    if ([topViewController isKindOfClass:[UINavigationController class]]) {
        UINavigationController * nav = (UINavigationController *)topViewController;
        [nav pushViewController:scanCodeViewController animated:YES];
    }
}


@end



@implementation NSString (DeviceScanCode)

-(NSString *)ker_scanType{
    
    /* 以下WXapi中的扫码类型 源生没有
     * CODABAR
     * RSS_14
     * RSS_EXPANDED
     * UPC_A
     * UPC_EAN_EXTENSION
     * WX_CODE
     * CODE_25
     */

    if ([self isEqualToString:AVMetadataObjectTypeQRCode]) {
        return @"QR_CODE";
    }else if ([self isEqualToString:AVMetadataObjectTypeAztecCode]){
        return @"AZTEC";
    }else if ([self isEqualToString:AVMetadataObjectTypeCode39Code]){
        return @"CODE_39";
    }else if ([self isEqualToString:AVMetadataObjectTypeCode93Code]){
        return @"CODE_93";
    }else if ([self isEqualToString:AVMetadataObjectTypeCode128Code]){
        return @"CODE_128";
    }else if ([self isEqualToString:AVMetadataObjectTypeDataMatrixCode ]){
        return @"DATA_MATRIX";
    }else if ([self isEqualToString:AVMetadataObjectTypeEAN8Code]){
        return @"EAN_8";
    }else if ([self isEqualToString:AVMetadataObjectTypeEAN13Code]){
        return @"EAN_13";
    }else if ([self isEqualToString:AVMetadataObjectTypeITF14Code]){
        return @"ITF";
    }else if ([self isEqualToString:AVMetadataObjectTypeDataMatrixCode]){
        return @"MAXICODE";
    }else if ([self isEqualToString:AVMetadataObjectTypePDF417Code]){
        return @"PDF_417";
    }else if ([self isEqualToString:AVMetadataObjectTypeUPCECode]){
        return @"UPC_E";
    }else {
        return @"UNKNOW_CODE";
    }

}

@end

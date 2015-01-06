package com.example.hellohalide;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.graphics.Bitmap;
import android.widget.ImageView;
import android.widget.Button;
import android.view.View;
import android.graphics.BitmapFactory;
import android.widget.TextView;
import android.graphics.Color;

public class CameraActivity extends Activity {
    private static final String TAG = "CameraActivity";

    private Bitmap bitmap;
    //private Bitmap bitmap2;
    private ImageView myImage;
    private ImageView myImage2;
    static {
        System.loadLibrary("native");
    }
    private static native int[] processFrame(int[] src, int w, int h);

    @Override
    public void onCreate(Bundle b) {
        super.onCreate(b);
        setContentView(R.layout.main);

        myImage = (ImageView)findViewById(R.id.myimage);
        myImage2 = (ImageView)findViewById(R.id.myimage2);
        TextView myImageWidth = (TextView)findViewById(R.id.myimagewidth);
        TextView myImageHeight = (TextView)findViewById(R.id.myimageheight);
        Button buttonProcess = (Button)findViewById(R.id.process);

        bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.cones);
        int bitmapWidth  = bitmap.getWidth();
        int bitmapHeight = bitmap.getHeight();

        myImage.setImageBitmap(bitmap);
        myImageWidth.setText("Width: " + String.valueOf(bitmapWidth));
        myImageHeight.setText("Height: " + String.valueOf(bitmapHeight));

        buttonProcess.setOnClickListener(new Button.OnClickListener(){
            @Override
            public void onClick(View arg0) {
                // TODO Auto-generated method stub
                bitmap = doProcess(bitmap);
                myImage2.setImageBitmap(bitmap);
                myImage2.invalidate();
            }});
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
    }

    private Bitmap doProcess(Bitmap src){
        int bmWidth = src.getWidth();
        int bmHeight = src.getHeight();
        int [] newBitmap = new int[bmWidth * bmHeight];
        int [] flatBitmap = new int[bmWidth * bmHeight * 3]; // for flatten RGB
        src.getPixels(newBitmap, 0, bmWidth, 0, 0, bmWidth, bmHeight);

        // flatten to 1D
        int index, alpha, r, g, b;
        int channel_size = bmWidth * bmHeight;
        for(int j=0; j<bmHeight; j++){
            for(int i=0; i<bmWidth; i++){
                index = j*bmWidth + i;
                alpha = Color.alpha(newBitmap[index]);
                r = Color.red(newBitmap[index]);
                g = Color.green(newBitmap[index]);
                b = Color.blue(newBitmap[index]);
                flatBitmap[index] = r;
                flatBitmap[1*channel_size + index] = g;
                flatBitmap[2*channel_size + index] = b;
            }// for(int i=0
        }// for(int j=0...

        //TODO: JNI call...
        int [] resultRGB = processFrame(flatBitmap, bmWidth, bmHeight);

        // pack into new Bitmap
        for(int j=0; j<bmHeight; j++){
            for(int i=0; i<bmWidth; i++){
                index = j*bmWidth+i;
                alpha = Color.alpha(newBitmap[index]);
                r = resultRGB[index];
                g = resultRGB[1*channel_size + index];
                b = resultRGB[2*channel_size + index];
                newBitmap[index] = Color.argb(alpha, r, g, b);
            }
        }
        Bitmap newbm = Bitmap.createBitmap(bmWidth, bmHeight, Bitmap.Config.ARGB_8888);
        newbm.setPixels(newBitmap, 0, bmWidth, 0, 0, bmWidth, bmHeight);
        return newbm;
    }// private Bitmap doProcess
}

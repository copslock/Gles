package com.app.jagles.util;


import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.os.Environment;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.text.SimpleDateFormat;
import java.util.Calendar;

public class ToolsUtil
{
    static final String fileDir = "xiaojiu";

	public static int loadTexture(final Context context, Bitmap mBitmap)
	{
		final int[] textureHandle = new int[1];
		GLES20.glGenTextures(1, textureHandle,0);
		
		if(textureHandle[0] != 0)
		{
			final Bitmap bitmap = mBitmap;
			GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureHandle[0]);
			
			GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
            bitmap.recycle();
		}
		if(textureHandle[0] == 0)
        {
            throw new RuntimeException("failed to load texture");
        }
		return textureHandle[0];
	}
	
    public static int loadTexture(final Context context, final int resourceId)
    {
        final int[] textureHandle = new int[1];
        GLES20.glGenTextures(1, textureHandle, 0);

        if(textureHandle[0] != 0)
        {
            final BitmapFactory.Options options = new BitmapFactory.Options();
            options.inScaled = false;

            final Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), resourceId, options);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureHandle[0]);

            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);

            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
            bitmap.recycle();
        }

        if(textureHandle[0] == 0)
        {
            throw new RuntimeException("failed to load texture");
        }
 
        return textureHandle[0];
    }

    public static int loadTextureError(String errortext){
        final int[] textureHandle = new int[1];
        GLES20.glGenTextures(1, textureHandle,0);
        if(textureHandle[0] != 0){
            Log.d("loadText", "create pointer success");
            final BitmapFactory.Options options = new BitmapFactory.Options();
            options.inScaled = false;
            final Bitmap bitmap = Bitmap.createBitmap(240,40, Bitmap.Config.RGB_565);
            Canvas canvas = new Canvas(bitmap);
            Paint p = new Paint();
            String s = errortext;
            String familyName = "Times New Roman";
            Typeface font = Typeface.create(familyName, Typeface.NORMAL);
            p.setColor(Color.WHITE);
            p.setTypeface(font);
            p.setTextSize(30);
            float width = p.measureText(s);
            canvas.drawText(s,(bitmap.getWidth() - width)/2,bitmap.getHeight()-10, p);

            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureHandle[0]);

            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);

            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
            bitmap.recycle();
        }
        if(textureHandle[0] == 0)
        {
            throw new RuntimeException("failed to load texture");
        }
        return textureHandle[0];
    }

    public static int loadTexture(final Context context, final int resourceId, final int index){
    	final int[] textureHandle = new int[1];
        GLES20.glGenTextures(1, textureHandle, 0);

        if(textureHandle[0] != 0)
        {
            final BitmapFactory.Options options = new BitmapFactory.Options();
            options.inScaled = false;
            final Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), resourceId, options).copy(Bitmap.Config.ARGB_8888, true);
            Canvas canvas = new Canvas(bitmap);
            Paint p = new Paint();
            String s = index+"";
            String familyName = "Times New Roman";
            Typeface font = Typeface.create(familyName, Typeface.NORMAL);
            p.setColor(Color.BLUE);
            p.setTypeface(font);
            p.setTextSize(50);
            float width = p.measureText(s);
            canvas.drawText(s,(bitmap.getWidth() - width)/2,bitmap.getHeight() - 30, p); 
            
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureHandle[0]);

            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);

            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
            bitmap.recycle();
        }

        if(textureHandle[0] == 0)
        {
            throw new RuntimeException("failed to load texture");
        }
 
        return textureHandle[0];
    }
    
    public static int loadTexture(final Context context, final int resourceId, final String errorText){
    	final int[] textureHandle = new int[1];
    	GLES20.glGenTextures(1, textureHandle, 0);
    	
    	if(textureHandle[0] != 0){
    		final BitmapFactory.Options options = new BitmapFactory.Options();
    		options.inScaled = false;
    		final Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), resourceId, options).copy(Bitmap.Config.ARGB_8888, true);
    		Canvas canvas = new Canvas(bitmap);
    		Paint p = new Paint();
    		String familyName = "Times New Roman";
    		Typeface font = Typeface.create(familyName, Typeface.NORMAL);
    		p.setColor(Color.WHITE);
    		p.setTypeface(font);
    		p.setTextSize(60);
    		float width = p.measureText(errorText);
    		canvas.drawText(errorText, (bitmap.getWidth()-width)/2, bitmap.getHeight()-10, p);
    		
    		GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureHandle[0]);
    		
    		GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_NEAREST);
    		GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_NEAREST);
    		
    		GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
    		Log.d("bitmap", "bitmap width......................"+bitmap.getWidth()+"  bitmap height.........."+bitmap.getHeight());
    		bitmap.recycle();
    	}
    	if(textureHandle[0] == 0)
		{
			throw new RuntimeException("failed to load texture");
		}
    	
    	return textureHandle[0];
    }
    
    /**
     * Helper function to compile a shader.
     *
     * @param shaderType The shader type.
     * @param shaderSource The shader source code.
     * @return An OpenGL handle to the shader.
     */
    public static int compileShader(final int shaderType, final String shaderSource)
    {
        int shaderHandle = GLES20.glCreateShader(shaderType);

        if (shaderHandle != 0)
        {
            // Pass in the shader source.
            GLES20.glShaderSource(shaderHandle, shaderSource);

            // Compile the shader.
            GLES20.glCompileShader(shaderHandle);

            // Get the compilation status.
            final int[] compileStatus = new int[1];
            GLES20.glGetShaderiv(shaderHandle, GLES20.GL_COMPILE_STATUS, compileStatus, 0);

            // If the compilation failed, delete the shader.
            if (compileStatus[0] == 0)
            {
                GLES20.glDeleteShader(shaderHandle);
                shaderHandle = 0;
            }
        }

        if (shaderHandle == 0)
        {
            throw new RuntimeException("Error creating shader.");
        }

        return shaderHandle;
    }

    /**
     * Helper function to compile and link a program.
     *
     * @param vertexShaderHandle An OpenGL handle to an already-compiled vertex shader.
     * @param fragmentShaderHandle An OpenGL handle to an already-compiled fragment shader.
     * @param attributes Attributes that need to be bound to the program.
     * @return An OpenGL handle to the program.
     */
    public static int createAndLinkProgram(final int vertexShaderHandle, final int fragmentShaderHandle, final String[] attributes)
    {
        int programHandle = GLES20.glCreateProgram();

        if (programHandle != 0)
        {
            // Bind the vertex shader to the program.
            GLES20.glAttachShader(programHandle, vertexShaderHandle);

            // Bind the fragment shader to the program.
            GLES20.glAttachShader(programHandle, fragmentShaderHandle);

            // Bind attributes
            if (attributes != null)
            {
                final int size = attributes.length;
                for (int i = 0; i < size; i++)
                {
                    GLES20.glBindAttribLocation(programHandle, i, attributes[i]);
                }
            }

            // Link the two shaders together into a program.
            GLES20.glLinkProgram(programHandle);

            // Get the link status.
            final int[] linkStatus = new int[1];
            GLES20.glGetProgramiv(programHandle, GLES20.GL_LINK_STATUS, linkStatus, 0);

            // If the link failed, delete the program.
            if (linkStatus[0] == 0)
            {
                GLES20.glDeleteProgram(programHandle);
                programHandle = 0;
            }
        }

        if (programHandle == 0)
        {
            throw new RuntimeException("Error creating program.");
        }

        return programHandle;
    }

    public static String readTextFileFromRawResource(final Context context,
                                                     final int resourceId)
    {
        final InputStream inputStream = context.getResources().openRawResource(
                resourceId);
        final InputStreamReader inputStreamReader = new InputStreamReader(
                inputStream);
        final BufferedReader bufferedReader = new BufferedReader(
                inputStreamReader);

        String nextLine;
        final StringBuilder body = new StringBuilder();

        try
        {
            while ((nextLine = bufferedReader.readLine()) != null)
            {
                body.append(nextLine);
                body.append('\n');
            }
        }
        catch (IOException e)
        {
            return null;
        }

        return body.toString();
    }

    public static String getSDCardPath()
    {
        String sdPath = Environment.getExternalStorageDirectory().getPath()+"/";
        File file = new File(sdPath+fileDir);
        if(!file.exists())
        {
            file.mkdir();
        }
        return sdPath+fileDir+"/";
    }

    public static String getThumbnailPath(Context mContext){
        String path = mContext.getFilesDir().getPath()+"/Thumbnail";
        File file = new File(path);
        if(!file.exists())
            try {
                file.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
            }
        return path;
    }

    /**
     * the file name type:2016.05.01-12:00:12
     * @return
     */
    public static String getFileName()
    {
        Calendar calendar = Calendar.getInstance();
        SimpleDateFormat format = new SimpleDateFormat("yyyy.MM.dd-HH.mm.ss");
        String fileName = format.format(calendar.getTime());
        return fileName;
    }

    public static String changedFileName(String oldFileName, String suffix)
    {
       String[] file_name_array = oldFileName.split("/");
        String[] file_name = file_name_array[2].split("-");
        String date = addPointToString(file_name[0]);
        String time = addPointToString(file_name[1]);
        return date+"-"+time+"."+suffix;
    }

    private static String addPointToString(String file_name)
    {
        StringBuilder  sb = new StringBuilder (file_name);
        sb.insert(file_name.length()-2, ".");
        sb.insert(file_name.length()-4, ".");
        return sb.toString();
    }
}
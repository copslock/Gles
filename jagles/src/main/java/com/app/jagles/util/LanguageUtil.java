package com.app.jagles.util;

import android.content.Context;

import java.util.Locale;

/**
 * Created by huangqinghe on 2017/11/2.
 */

public class LanguageUtil {

    /**
     * 判断中英文
     *
     * @return
     */
    public static boolean isZh(Context context) {
        Locale locale = context.getResources().getConfiguration().locale;
        String language = locale.getLanguage();
        if (language.endsWith("zh")) {
            return true;
        } else {
            return false;
        }
    }
}

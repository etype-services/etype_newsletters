<?php

// autoload_static.php @generated by Composer

namespace Composer\Autoload;

class ComposerStaticInit7e54d47d1bcbf3604615428b8563bfee
{
    public static $prefixLengthsPsr4 = array (
        'T' => 
        array (
            'Twig\\' => 5,
        ),
    );

    public static $prefixDirsPsr4 = array (
        'Twig\\' => 
        array (
            0 => __DIR__ . '/..' . '/twig/twig/src',
        ),
    );

    public static $prefixesPsr0 = array (
        'T' => 
        array (
            'Twig_' => 
            array (
                0 => __DIR__ . '/..' . '/twig/twig/lib',
            ),
        ),
    );

    public static function getInitializer(ClassLoader $loader)
    {
        return \Closure::bind(function () use ($loader) {
            $loader->prefixLengthsPsr4 = ComposerStaticInit7e54d47d1bcbf3604615428b8563bfee::$prefixLengthsPsr4;
            $loader->prefixDirsPsr4 = ComposerStaticInit7e54d47d1bcbf3604615428b8563bfee::$prefixDirsPsr4;
            $loader->prefixesPsr0 = ComposerStaticInit7e54d47d1bcbf3604615428b8563bfee::$prefixesPsr0;

        }, null, ClassLoader::class);
    }
}

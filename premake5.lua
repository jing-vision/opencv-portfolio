-- https://github.com/premake/premake-core/wiki

local action = _ACTION or ""
local CUDA_PATH = os.getenv("CUDA_PATH");

solution "opencv-portfolio"
    location ("_project")
    configurations { "Debug", "Release" }
    platforms {"x64", "x86"}
    language "C++"
    kind "ConsoleApp"

    configuration "vs*"
        defines { "_CRT_SECURE_NO_WARNINGS" }

        configuration "x86"
            libdirs {
                "../opencv-lib/vs2013-x86",
                "../opencv-gfx/x86",
                "x86",
            }
            targetdir ("x86")

        configuration "x64"
            libdirs {
                "../opencv-lib/vs2013-x64",
                "../opencv-gfx/x64",
                "x64",
            }
            targetdir ("x64")

        os.mkdir("x86");
        os.copyfile("../opencv-lib/vs2013-x86/opencv_world300d.dll", "x86/opencv_world300d.dll")
        os.copyfile("../opencv-lib/vs2013-x86/opencv_world300.dll", "x86/opencv_world300.dll")
        os.mkdir("x64");
        os.copyfile("../opencv-lib/vs2013-x64/opencv_world300d.dll", "x64/opencv_world300d.dll")
        os.copyfile("../opencv-lib/vs2013-x64/opencv_world300.dll", "x64/opencv_world300.dll")
        os.copyfile("../opencv-lib/vs2013-x64/OpenNI2.dll", "x64/OpenNI2.dll")

    flags {
        "MultiProcessorCompile"
    }

    configuration "Debug"
        links {
            "opencv_world300d.lib"
        }

    configuration "Release"
        links {
            "opencv_world300.lib"
        }

    configuration "Debug"
        defines { "DEBUG" }
        flags { "Symbols"}
        targetsuffix "-d"

    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize"}

-- 
    project "opencv-portfolio"
        kind "StaticLib"

        includedirs {
            "include",
            "src",
            "../opencv-lib/include",
        }

        files {
            "include/opencv2/*",
            "src/*",
        }

        defines {

        }

    function create_app_project( app_path )
        leaf_name = string.sub(app_path, string.len("apps/") + 1);

        project (leaf_name)

            includedirs {
                "include",
                "../opencv-lib/include",
                "apps/" .. leaf_name .. "/include/**",
            }

            if CUDA_PATH ~= nil then
                includedirs { 
                    path.join("$(CUDA_PATH)", "include"),
                }
                links {
                    "cuda.lib",
                    "cudart.lib",
                    "nvrtc.lib"
                }
                configuration {"x86", "windows"}
                    libdirs {
                        path.join("$(CUDA_PATH)", "lib/win32"),
                    }
                configuration {"x64", "windows"}
                    libdirs {
                        path.join("$(CUDA_PATH)", "lib/x64"),
                    }
            end

            files {
                "apps/" .. leaf_name .. "/**",
            }

            links {
                "opengl32.lib",
                "glu32.lib",
            }
    end

    local apps = os.matchdirs("apps/*")
    for _, app in ipairs(apps) do
        create_app_project(app)
    end

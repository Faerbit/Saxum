-- Inits search path for require.
-- This file needs to stay next to the other scripts.

package.path = debug.getinfo(1,"S").source:match[[^@?(.*[\/])[^\/]-$]] .."?.lua;".. package.path

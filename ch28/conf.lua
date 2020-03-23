-- define window size
width = 200
height = 300

background = {red = 0.30, green = 0.10, blue = 0}
-- background = "WHITE"

function f(x, y)
    return (x^2 * math.sin(y)) / (1 - x)
end

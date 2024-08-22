from fasthtml.common import *

css = Style('.square-dark{background-color: gray} .square-light{background-color: antiquewhite} :root{--pico-grid-column-gap: 0px; --pico-grid-row-gap: 0px}')

app = FastHTML(hdrs=(picolink, css))

SQUARE_SIZE = "100px"

def create_row(start_with_dark, row):
    colours = ['square-dark', 'square-light'] if start_with_dark else ['square-light', 'square-dark']
    return [Div(id=f'b{row}{col}', cls=colours[col % 2], style=f'height: {SQUARE_SIZE};') for col in range(8)]

rows = [Grid(*create_row(row % 2 == 0, row)) for row in range(8)]
board = Div(*rows, style='width: 800px') 

@app.get("/")
def home():
    return Div(H1('Hello, World'), board)

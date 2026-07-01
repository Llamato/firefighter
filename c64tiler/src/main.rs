use std::fmt::Display;
use std::path::{Path, PathBuf};
use std::env::Args;
static RETURNCODE_PARAMETER_MISSING: i32 = 1;
static SCREEN_WIDTH: u16 = 40;
static SCREEN_HEIGHT: u16 = 25;
static BYTES_PER_CELL: u8 = 8;
static BITMAP_SIZE: usize = SCREEN_WIDTH as usize * SCREEN_HEIGHT as usize * BYTES_PER_CELL as usize;
static COLORMAP_SIZE: usize = SCREEN_WIDTH as usize * SCREEN_HEIGHT as usize;

struct Parameters {
    pub x: u8,
    pub y: u8,
    pub width: u8,
    pub height: u8,
    pub input_file: PathBuf,
    pub bitmap_output_file: PathBuf,
    pub color_output_file: PathBuf
}
impl Parameters {
    fn process_args<T>(args: &mut Args, index: usize, msg: &str) -> T where T: std::str::FromStr, <T as std::str::FromStr>::Err: Display, {
        return args.nth(index).unwrap_or_else(|| exit_on_missing_parameter(msg)).parse::<T>()    
        .map_err(|e| e.to_string())
    .unwrap()
    }

    pub fn new(mut args: Args) -> Self {

        return Self{
            x: Self::process_args(&mut args, 1, "Provide x coordinate of tile"),
            y: Self::process_args(&mut args, 2, "Provide y coordinate of tile"),
            width: Self::process_args(&mut args, 3, "Provide width of tile"),
            height: Self::process_args(&mut args, 4, "Provide height of tile"),
            input_file: Self::process_args(&mut args, 5, "Provide input file"),
            bitmap_output_file: Self::process_args(&mut args, 6, "Provide bitmap output file"),
            color_output_file: Self::process_args(&mut args, 7, "Provide color output file")
        };
    }
}

struct CbmBitmap {
    bitmap_data: Vec<u64>,
    color_data: Vec<u8>
}
impl CbmBitmap {
    pub fn new(buffer: Vec<u8>) -> Self {
        let bitmap_data = buffer[..BITMAP_SIZE]
            .chunks_exact(BYTES_PER_CELL as usize)
            .map(|chunk| u64::from_le_bytes(chunk.try_into().unwrap()))
        .collect();
        let color_data: Vec<u8> = buffer[BITMAP_SIZE..COLORMAP_SIZE].to_vec();
        return Self{bitmap_data, color_data};
    }

    pub fn extract_rectangle(&self, x: u8, y: u8, width: u8, height: u8) -> CbmBitmap {
        let (x, y) = (x as usize, y as usize);
        let tile_start = y * SCREEN_WIDTH as usize + x;
        let bitmap_slices : Vec<&[u64]> = (0..height)
            .map(|y| {
                let start = (tile_start + SCREEN_WIDTH as usize * y as usize) as usize;
                &self.bitmap_data[start..start+(width as usize)]
            }).collect();
            let new_bitmap_data = bitmap_slices.concat();

        let color_slices : Vec<&[u8]> = (0..height)
            .map(|y| {
                let start = (tile_start + SCREEN_WIDTH as usize * y as usize) as usize;
                &self.color_data[start..start+(width as usize)]
            }).collect();
            let new_color_data = color_slices.concat();
            
        return Self { bitmap_data: new_bitmap_data, color_data: new_color_data };
    }
}

fn exit_on_missing_parameter(msg : &str) -> String {
    println!("{}", msg);
    std::process::exit(RETURNCODE_PARAMETER_MISSING);
}

fn main() {
    let parameters = Parameters::new(std::env::args());
    let raw_input_data = match std::fs::read(parameters.input_file) {
        Ok(data) => {data[2..].to_vec()}
        Err(e) => {panic!("Reading input file failed with {}", e)}
    };
    let full_bitmap = CbmBitmap::new(raw_input_data);
    
    let tileset = full_bitmap.extract_rectangle(parameters.x, parameters.y, parameters.width, parameters.height);
    println!("{:?}", tileset.bitmap_data)
}

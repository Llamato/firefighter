use std::fmt::Display;
use std::path::PathBuf;
use std::env::Args;
 
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
    fn process_args<T>(args: &mut Args) -> Result<T, String>
    where
        T: std::str::FromStr,
        T::Err: Display,
    {
        let arg = args.next().ok_or("missing argument")?;
        arg.parse::<T>().map_err(|e| e.to_string())
    }
 
    pub fn new() -> Result<Self, String> {
        let mut args = std::env::args();
        args.next(); // skip executable
 
        Ok(Self {
            x: Self::process_args(&mut args)
                .map_err(|e| format!("Provide x coordinate of tile: {e}"))?,
            y: Self::process_args(&mut args)
                .map_err(|e| format!("Provide y coordinate of tile: {e}"))?,
            width: Self::process_args(&mut args)
                .map_err(|e| format!("Provide width of tile: {e}"))?,
            height: Self::process_args(&mut args)
                .map_err(|e| format!("Provide height of tile: {e}"))?,
            input_file: Self::process_args(&mut args)
                .map_err(|e| format!("Provide input file: {e}"))?,
            bitmap_output_file: Self::process_args(&mut args)
                .map_err(|e| format!("Provide bitmap output file: {e}"))?,
            color_output_file: Self::process_args(&mut args)
                .map_err(|e| format!("Provide color output file: {e}"))?,
        })
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
 
fn main() -> Result<(), String> {
    let parameters = Parameters::new()?;
 
    let raw_input_data = std::fs::read(parameters.input_file)
        .map_err(|e| e.to_string())?;
 
    let full_bitmap = CbmBitmap::new(raw_input_data[2..].to_vec());
 
    let tileset = full_bitmap.extract_rectangle(parameters.x, parameters.y, parameters.width, parameters.height);
    println!("{:?}", tileset.bitmap_data);
 
    Ok(())
}
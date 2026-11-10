use crate::pcg::Pcg32;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Seed(pub u128);

impl Default for Seed {
    fn default() -> Self {
        Self(274369434223529508508286369196229651910)
    }
}

impl Seed {
    #[cfg(not(miri))]
    pub fn new() -> Self {
        let now = std::time::SystemTime::now();

        match now.duration_since(std::time::UNIX_EPOCH) {
            Ok(duration_since_epoch) => {
                let millis = duration_since_epoch.as_millis();
                let seed = Seed(millis);

                // generate a better seed
                let mut rng = Rng::new(seed);
                let better_seed_value = rng.next_u128();
                Self(better_seed_value)
            }
            Err(_) => Seed::default(),
        }
    }

    #[cfg(miri)]
    pub fn new() -> Self {
        Self::default()
    }

    pub fn zero() -> Self {
        Seed(0)
    }
}

#[derive(Debug)]
pub struct Rng {
    seed: Seed,
    pcg: Pcg32,
}

impl Rng {
    pub fn new(seed: Seed) -> Rng {
        let pcg = Pcg32::new_from_seed(seed.0);
        let mut result = Rng { seed, pcg };
        result.skip(128);
        result
    }

    pub fn seed(&self) -> Seed {
        self.seed
    }

    // advance internal state n times. useful for warming up the generator
    pub fn skip(&mut self, n: usize) {
        for _ in 0..n {
            self.pcg.next();
        }
    }

    /// returns a random u8
    pub fn next_u8(&mut self) -> u8 {
        let x = self.next_u32();
        (x & 0xFF) as u8
    }

    /// fills a buffer with random u8s
    pub fn next_u8s(&mut self, buf: &mut [u8]) {
        for entry in buf.iter_mut() {
            *entry = self.next_u8();
        }
    }

    /// returns a random u16
    pub fn next_u16(&mut self) -> u16 {
        let x = self.next_u32();
        let x_ = x & 0xFFFF;
        x_ as u16
    }

    /// returns a random u32
    pub fn next_u32(&mut self) -> u32 {
        self.pcg.next()
    }

    /// returns a random u64
    pub fn next_u64(&mut self) -> u64 {
        let a: u64 = self.next_u32().into();
        let b: u64 = self.next_u32().into();
        (a << 32) | b
    }

    /// returns a random u128
    pub fn next_u128(&mut self) -> u128 {
        let a: u128 = self.next_u64().into();
        let b: u128 = self.next_u64().into();
        (a << 64) | b
    }

    /// returns a random usize
    pub fn next_usize(&mut self) -> usize {
        const SIZE: usize = std::mem::size_of::<usize>();
        let mut bytes = [0u8; SIZE];
        self.next_u8s(&mut bytes);
        usize::from_ne_bytes(bytes)
    }

    /// returns a random i32
    pub fn next_i32(&mut self) -> i32 {
        let x = self.next_u32();
        i32::from_ne_bytes(x.to_ne_bytes())
    }

    /// returns a random isize
    pub fn next_isize(&mut self) -> isize {
        let x = self.next_usize();
        isize::from_ne_bytes(x.to_ne_bytes())
    }

    /// returns a random bool
    pub fn next_bool(&mut self) -> bool {
        let x = self.next_u32();
        (x & 1) == 1
    }

    // returns a f32 between 0.0 and 1.0, using a hash
    pub fn hash_to_f32(value: u32) -> f32 {
        f32::from_bits(0x3F80_0000 | (value & 0x7F_FFFF)) - 1.0
    }

    /// returns a random f32 between 0.0 and 1.0
    pub fn next_f32(&mut self) -> f32 {
        let x = self.next_u32();
        Self::hash_to_f32(x)
    }

    /// returns a random f32 between min and max
    pub fn next_f32_between(&mut self, min: f32, max: f32) -> f32 {
        if max <= min {
            if max == min {
                return min;
            } else {
                return f32::NAN;
            }
        }

        let x = self.next_f32();
        let r = (max - min) * x + min;

        if r > max { max } else { r }
    }

    /// min and max are inclusive
    pub fn next_i32_between(&mut self, min: i32, max: i32) -> i32 {
        let max = max + 1;
        if max <= min {
            if max == min {
                return min;
            } else {
                return i32::MIN;
            }
        }

        let x = self.next_f32();
        let r = (((max - min) as f32) * x) as i32 + min;

        if r > max { max } else { r }
    }

    /// returns a random element in a slice.
    pub fn next_in_slice<'a, T>(&mut self, slice: &'a [T]) -> Option<&'a T> {
        if slice.is_empty() {
            return None;
        }

        let min = 0;
        let max = (slice.len() - 1) as i32;
        let index = self.next_i32_between(min, max) as usize;
        slice.get(index)
    }
}

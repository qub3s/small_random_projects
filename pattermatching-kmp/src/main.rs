// Dependencies
// bytes = "1.3.0" 

fn kmp(t : bytes::Bytes, p : bytes::Bytes, mut result : Vec<i32>) ->Vec<i32> {
    let mut ppj : Vec<i32> = vec![0i32;p.len()+1];    // pre-processed-jumps
    
    let mut i = 0;
    let mut j = -1;
    ppj[0] = -1;

    // building the table (ppj)
    while i < p.len(){
        while j >= 0 && p[i] != p[j as usize]{
            j = ppj[j as usize];
        }
        i = i+1;
        j = j+1;
        ppj[i] = j;
    }

    i = 0;
    j = 0;

    // searching for the word in the text
    while i < t.len() as usize{
        while j >= 0 && t[i] != p[j as usize]{
            j = ppj[j as usize];
        }
        i = i + 1;
        j = j + 1;

        if j == p.len() as i32{
            result.push(i as i32);              // writing the position of the word in the result vector
            j = ppj[j as usize];
        }
    }
    return result;
}

fn main(){
    print!( "{:?}", kmp(bytes::Bytes::from_static(b"this is some random text where i will search for the char combination om"), bytes::Bytes::from_static(b"om"), Vec::new() ) );   
}

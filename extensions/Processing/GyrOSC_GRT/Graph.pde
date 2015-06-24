class Graph{
  
  int bufferSize;
  int numDimensions;
  float[][] data;
  float[] axisMin;
  float[] axisMax;
  color[] axisColors;
  String name;
  boolean initialized;
  
  Graph(int numDimensions,int bufferSize, String name,color[] axisColors,float[] axisMin,float[] axisMax){
    initialized = false;
    init( numDimensions, bufferSize, name, axisColors, axisMin, axisMax );
  }
  
  boolean init(int numDimensions,int bufferSize, String name,color[] axisColors,float[] axisMin,float[] axisMax){
    
    initialized = false;
    
    if( numDimensions < 1 ){
      return false;
    }
    
    if( bufferSize < 1 ){
      return false;
    }
    
    if( axisColors.length != numDimensions || axisMin.length != numDimensions || axisMax.length != numDimensions ){
          return false;
    }
        
    this.numDimensions = numDimensions;
    this.bufferSize = bufferSize;
    this.name = name;
    this.data = new float[ bufferSize ][ numDimensions ];
    this.axisColors = new color[ numDimensions ];
    this.axisMin = new float[ numDimensions ];
    this.axisMax = new float[ numDimensions ];
    
    for (int j = 0; j < numDimensions; j++) {
      this.axisMin[ j ] = axisMin[ j ];
      this.axisMax[ j ] = axisMax[ j ];
      this.axisColors[ j ] = axisColors[ j ];
    }
 
   initialized = true;
    
    return true; 
  }
  
  boolean update(float[] sample){
    if(  !initialized ) return false;
    if( sample.length != numDimensions ) return false;
    
    //Move all the values down
    for (int i = 0; i<bufferSize-1; i++) {
      for (int j = 0; j<numDimensions; j++) {
        data[i][j] = data[i+1][j]; 
      }
    }
    
    //Add the new sample
    for (int j = 0; j < numDimensions; j++) {
      data[ bufferSize-1 ][ j ] = sample[ j ]; 
    }
    
    return true;
  }
  
  
  boolean draw(float graphX,float graphY,float graphWidth,float graphHeight){
    
      if(  !initialized ) return false;
      
      fill( 0 );
      textFont(font, 12);
      text(name,graphX + 10,graphY + 15);
      
      float N = bufferSize;
      strokeWeight(1.5);
      for (int i = 0; i < bufferSize-1; i++) {
        for( int j=0; j< numDimensions; j++){
           stroke( axisColors[ j ] );
           float x1 = graphX + (i/N * graphWidth);
           float x2 = graphX + ((i+1)/N * graphWidth);
           float y1 = graphY + map(data[i][j],axisMin[j],axisMax[j],0,graphHeight);
           float y2 = graphY + map(data[i+1][j],axisMin[j],axisMax[j],0,graphHeight);
           line(x1,y1,x2,y2);
        }
      }
      
      noFill();
      stroke(0);
      strokeWeight(1);
      rect(graphX,graphY,graphWidth,graphHeight);
      
      return true;
  }
  
}

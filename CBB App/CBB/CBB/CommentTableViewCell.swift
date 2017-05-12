//
//  CommentTableViewCell.swift
//  CBB
//
//  Created by Erich Chu on 4/23/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import UIKit

class CommentTableViewCell: UITableViewCell {

    @IBOutlet weak var commentPoster: UILabel!
    @IBOutlet weak var timestamp: UILabel!
    @IBOutlet weak var commentBody: UITextView!
    
    override func awakeFromNib() {
        super.awakeFromNib()
        // Initialization code
    }

    override func setSelected(_ selected: Bool, animated: Bool) {
        super.setSelected(selected, animated: animated)

        // Configure the view for the selected state
    }

}
